#include "RenderPipeline.h"
#include "ViewTransform.h"
#include "ViewportTransform.h"
#include "modeltransform.h"
#include "ProjectionTransform.h"

#include "../TwoWayRender/include/threadpool.h"
#include "Shader.h"
#include "Texture.h"
static auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
	return Vector4f(v3.x(), v3.y(), v3.z(), w);
}

struct light
{
	Eigen::Vector3f position;
	Eigen::Vector3f intensity;
};
static Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload)
{
	Eigen::Vector3f return_color = { 0, 0, 0 };
	if (payload.texture)
	{
		return_color = payload.texture->getColor(payload.tex_coords.x(), payload.tex_coords.y());
	}
	Eigen::Vector3f texture_color;
	texture_color << return_color.x(), return_color.y(), return_color.z();

	Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
	Eigen::Vector3f kd = texture_color / 255.f;
	Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

	auto l1 = light{ {20, 20, 20}, {500, 500, 500} };
	auto l2 = light{ {-20, 20, 0}, {500, 500, 500} };

	std::vector<light> lights = { l1, l2 };
	Eigen::Vector3f amb_light_intensity{ 10, 10, 10 };
	Eigen::Vector3f eye_pos{ 0, 0, 10 };

	float p = 150;

	Eigen::Vector3f color = texture_color;
	Eigen::Vector3f point = payload.view_pos;
	Eigen::Vector3f normal = payload.normal;

	Eigen::Vector3f result_color = { 0, 0, 0 };
	Eigen::Vector3f ambient = ka * amb_light_intensity[0];

	for (auto& light : lights)
	{
		// TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
		// components are. Then, accumulate that result on the *result_color* object.
		float r2 = (light.position - point).dot((light.position - point));
		Eigen::Vector3f l = (light.position - point).normalized();
		Eigen::Vector3f n = normal.normalized();
		Eigen::Vector3f v = (eye_pos - point).normalized();
		Eigen::Vector3f h = (l + v).normalized();

		Eigen::Vector3f diffuse = (kd * light.intensity[0] / r2) * std::max(0.0f, n.dot(l));
		Eigen::Vector3f specular = (ks * light.intensity[0] / r2) * std::pow(std::max(0.f, (n.dot(h))), p);

		result_color += (diffuse + specular);
	}
	result_color += ambient;
	return result_color;
}

RenderPipeline::RenderPipeline() {

	m_threadpool = std::make_unique<ThreadPool>(std::thread::hardware_concurrency());
	m_threadpool->Start();
}

void RenderPipeline::SetModelTransform(std::shared_ptr<ModelTransform> in_modeltransformer) {
	m_modeltransformer = in_modeltransformer;
}
void RenderPipeline::SetViewTransform(std::shared_ptr<ViewTransform> in_camera) {
	m_camera = in_camera;
}
void RenderPipeline::SetProjectionTransform(std::shared_ptr<ProjectionTransform> in_projection) {
	m_projection = in_projection;
}
void RenderPipeline::SetViewportTransform(std::shared_ptr<ViewportTransform> in_viewport) {
	m_viewport = in_viewport;
}
void RenderPipeline::SetTextureImage(std::shared_ptr<TextureImage> in_textureImage)
{
	m_textureImage = in_textureImage;
}



Eigen::Matrix4f RenderPipeline::GetMvpMatrix() {
	
	auto m = m_modeltransformer->getModelMatrix();
	auto v = m_camera->getViewMatrix();
	auto p = m_projection->getPerspectiveMatrix();

	return p * v * m;

}
Eigen::Matrix4f RenderPipeline::GetMvMatrix() {
	auto m = m_modeltransformer->getModelMatrix();
	auto v = m_camera->getViewMatrix();

	return v * m;
}
std::vector<RenderPipeline::TriWithAABB> RenderPipeline::AfterMvpTransform(const std::vector<std::shared_ptr<Triangle>>& triangle_array) {
	Eigen::Matrix4f mvp = GetMvpMatrix();
	Eigen::Matrix4f mv = GetMvMatrix();
	Eigen::Matrix4f inv_trans = (mv).inverse().transpose();
	Eigen::Matrix4f viewport_matrix = m_viewport->GetMatrix();
	int m_triangles_size = triangle_array.size();
	std::vector<TriWithAABB> triwithAABB(m_triangles_size, {});
	for (int i = 0; i < m_triangles_size; ++i)
	{
		auto t = triangle_array[i];
		Triangle newtri = *t;

		// ========== 1.计算MV，得到观察空间坐标，用于光照 ==========
		std::array<Eigen::Vector4f, 3> mm{
			mv * t->v[0],
			mv * t->v[1],
			mv * t->v[2]
		};
		std::array<Eigen::Vector3f, 3> viewspace_pos;
		std::transform(mm.begin(), mm.end(), viewspace_pos.begin(),
			[](auto& v) {return v.template head<3>(); });

		// ==========2.MVP变换 + 透视除法 ==========
		Eigen::Vector4f v[] = {
			mvp * t->v[0],
			mvp * t->v[1],
			mvp * t->v[2]
		};
		for (auto& vec : v) {
			vec.x() /= vec.w();
			vec.y() /= vec.w();
			vec.z() /= vec.w();
			vec.w() = 1.0f; // 透视除法之后w置1，再送入视口变换
		}

		// ✅修复：视口变换输入是【做完mvp+透视除法的v】，不要再用t->v[]原始顶点！
		Eigen::Vector4f viwport[] = {
			viewport_matrix * v[0],
			viewport_matrix * v[1],
			viewport_matrix * v[2]
		};

		// =========法线变换不变==========
		Eigen::Vector4f n[] = {
			inv_trans * to_vec4(t->normal[0], 0.0f),
			inv_trans * to_vec4(t->normal[1], 0.0f),
			inv_trans * to_vec4(t->normal[2], 0.0f)
		};

		for (int k = 0; k < 3; ++k)
			newtri.setVertex(k, viwport[k]);
		for (int k = 0; k < 3; ++k)
			newtri.setNormal(k, n[k].head<3>());

		newtri.setColor(0, 148, 121.0, 92.0);
		newtri.setColor(1, 148, 121.0, 92.0);
		newtri.setColor(2, 148, 121.0, 92.0);

		float yminf = std::min({ newtri.v[0].y(), newtri.v[1].y(), newtri.v[2].y() });
		float ymaxf = std::max({ newtri.v[0].y(), newtri.v[1].y(), newtri.v[2].y() });

		struct TriWithAABB temp;
		temp.tri = newtri;
		temp.ymin = yminf;
		temp.ymax = ymaxf;
		temp.view_pos = viewspace_pos;
		triwithAABB[i] = temp;
	}
	return triwithAABB;
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector4f* v) {
	float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
	float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
	float c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
	return { c1,c2,c3 };
}

static Eigen::Vector3f interpolate(float alpha, float beta, float gamma, const Eigen::Vector3f& vert1, const Eigen::Vector3f& vert2, const Eigen::Vector3f& vert3, float weight)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3) / weight;
}

void RenderPipeline::Render(const std::vector<std::shared_ptr<Triangle>>& triangle_array, std::shared_ptr<QImage> image) {
	m_image = image;
	std::vector<float> depth_buf(m_image->height()*m_image->width());
	m_image->fill(Qt::gray);
	std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::max());

	int ImageWidth = image->width();
	int ImageHeight = image->height();

	std::vector<TriWithAABB> triwithAABB = AfterMvpTransform(triangle_array);

	auto start_time = std::chrono::high_resolution_clock::now();

	const int BLOCK_ROWS = 8;
	std::vector<std::future<void>> futures;
	for (int y_begin = 0; y_begin < ImageHeight; y_begin += BLOCK_ROWS)
	{
		int y_end = std::min(y_begin + BLOCK_ROWS, ImageHeight);

		auto fut = m_threadpool->AddRetTask([this, y_begin, y_end, ImageWidth, ImageHeight,
			&triwithAABB,&depth_buf]() mutable -> void
			{
				for (int y = y_begin; y < y_end; ++y)
				{
					for (size_t tri_idx = 0; tri_idx < triwithAABB.size(); tri_idx++)
					{
						const Triangle& tri = triwithAABB[tri_idx].tri;
						const auto& view_pos = triwithAABB[tri_idx].view_pos;

						auto v = tri.toVector4();
						float yminf = triwithAABB[tri_idx].ymin;
						float ymaxf = triwithAABB[tri_idx].ymax;

						if (y < yminf - 1e-6f || y > ymaxf + 1e-6f)
							continue;

						auto x_array = getScanIntersectX(tri, static_cast<double>(y));
						if (x_array.size() <= 1)
							continue;

						int left = static_cast<int>(std::ceil(x_array[0]));
						int right = static_cast<int>(std::floor(x_array[1]));

						for (int x = left; x <= right; ++x)
						{
							auto [alpha, beta, gamma] = computeBarycentric2D(x + 0.5f, y + 0.5f, tri.v);

							float Z = 1.0f / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
							float zp = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
							zp *= Z;

							int idx = get_index(x, y);
							if (zp < depth_buf[idx])
							{
								depth_buf[idx] = zp;

								auto interpolated_color = interpolate(alpha, beta, gamma, tri.color[0], tri.color[1], tri.color[2], 1);
								auto interpolated_normal = interpolate(alpha, beta, gamma, tri.normal[0], tri.normal[1], tri.normal[2], 1).normalized();
								auto interpolated_texcoords = alpha * tri.tex_coords[0] + beta * tri.tex_coords[1] + gamma * tri.tex_coords[2];
								auto interpolated_shadingcoords = interpolate(alpha, beta, gamma, view_pos[0], view_pos[1], view_pos[2], 1);

								fragment_shader_payload payload(interpolated_color, interpolated_normal, interpolated_texcoords,
									m_textureImage ? m_textureImage.get() : nullptr);
								payload.view_pos = interpolated_shadingcoords;
								auto pixel_color = texture_fragment_shader(payload);

								set_pixel(Eigen::Vector2i(x, y), pixel_color);
							}


						}
					}
				}
			});
		futures.push_back(std::move(fut));
	}

	// 等待本帧所有光栅化任务全部完成
	for (auto& f : futures)
	{
		if (f.valid())
			f.wait();
	}

	auto end_time = std::chrono::high_resolution_clock::now();
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
	float fps = 0;
	if(seconds.count()!=0)
	{
		float fps = 1 / seconds.count();
	}
	
	drawFpsOnImage(fps);
	static int id = 0;
	++id;
	QString name = QString(std::to_string(id).c_str()) + "image.png";
	bool state = m_image->save(name);
}

void RenderPipeline::drawFpsOnImage(double fps)
{
	if (m_image->isNull())
		return;

	QPainter painter(m_image.get());

	// 抗锯齿
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::TextAntialiasing);

	// 设置字体
	QFont font;
	font.setPixelSize(24);      //字号，按需调
	font.setBold(true);
	painter.setFont(font);

	// 黄色画笔
	painter.setPen(QColor(255, 0, 0)); // 纯黄色

	// 左上角绘制，坐标(5,25)，留一点边距，避免贴住边框
	QString text = QString("FPS: %1").arg(fps, 0, 'f', 1); //保留1位小数
	painter.drawText(5, 25, text);

	painter.end();
}
void RenderPipeline::set_pixel(const Eigen::Vector2i& point, const Eigen::Vector3f& color)
{
	int y =  point.y(); //计算的数学坐标系 原点是左下角  Qt 图片原点是左上角
	int x = point.x();
	if (x < 0 || x >= m_image->width() || y < 0 || y >= m_image->height())
		return;

	int r = std::clamp((int)(255.0f * color[0]), 0, 255);
	int g = std::clamp((int)(255.0f * color[1]), 0, 255);
	int b = std::clamp((int)(255.0f * color[2]), 0, 255);

	QRgb* line = reinterpret_cast<QRgb*>(m_image->scanLine(y));
	line[x] = qRgb(r, g, b);
}
std::vector<double> RenderPipeline::getScanIntersectX(const Triangle& tri, double y_scan)
{
	std::vector<double> xs;
	// 三条边：0‑1，1‑2，2‑0
	const std::array<std::pair<int, int>, 3> edges{ {{0,1}, {1,2}, {2,0}} };

	constexpr double eps = 1e-8;

	for (const auto& e : edges)
	{
		int i0 = e.first;
		int i1 = e.second;
		const Vector4f& p0 = tri.v[i0];
		const Vector4f& p1 = tri.v[i1];

		double y0 = static_cast<double>(p0.y());
		double y1 = static_cast<double>(p1.y());

		// 水平边直接跳过，避免重复交点
		if (std::abs(y1 - y0) < eps)
			continue;

		// 保证 p0.y < p1.y
		const Vector4f* pa = &p0;
		const Vector4f* pb = &p1;
		if (y0 > y1)
		{
			std::swap(pa, pb);
			std::swap(y0, y1);
		}

		// 半开区间 [y0 , y1)，光栅化标准：包含上边，排除下边，消除裂缝
		if (y_scan < y0 - eps || y_scan >= y1)
			continue;

		// 插值系数 t
		double t = (y_scan - y0) / (y1 - y0);
		double x0 = static_cast<double>(pa->x());
		double x1 = static_cast<double>(pb->x());
		double x_intersect = x0 + t * (x1 - x0);

		xs.push_back(x_intersect);
	}

	// x从小到大排序
	std::sort(xs.begin(), xs.end());
	return xs;
}


int RenderPipeline::get_index(int x, int y)
{
	return (m_image->height() - y) * m_image->width() + x;
}
