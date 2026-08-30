#include "RasterizationDevice.h"
#include <QPainter>

#include "MVPTransformer.h"
#include <array>
#include <algorithm>
#include <limits>
#include "Shader.h"
#include "Texture.h"
//TGAColor sample2D(const TGAImage& img, const vec2& uvf) {
//	return img.get(uvf[0] * img.width(), uvf[1] * img.height());
//}

struct light
{
	Eigen::Vector3f position;
	Eigen::Vector3f intensity;
};
auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
	return Vector4f(v3.x(), v3.y(), v3.z(), w);
}
static Eigen::Vector3f interpolate(float alpha, float beta, float gamma, const Eigen::Vector3f& vert1, const Eigen::Vector3f& vert2, const Eigen::Vector3f& vert3, float weight)
{
	return (alpha * vert1 + beta * vert2 + gamma * vert3) / weight;
}

Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload)
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
	return result_color ;
}

Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload)
{
	Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
	Eigen::Vector3f kd = payload.color;
	Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

	auto l1 = light{ {20, 20, 20}, {500, 500, 500} };
	auto l2 = light{ {-20, 20, 0}, {500, 500, 500} };

	std::vector<light> lights = { l1, l2 };
	Eigen::Vector3f amb_light_intensity{ 10, 10, 10 };
	Eigen::Vector3f eye_pos{ 0, 0, 10 };

	float p = 150;

	Eigen::Vector3f color = payload.color;
	Eigen::Vector3f point = payload.view_pos;
	Eigen::Vector3f normal = payload.normal;

	Eigen::Vector3f result_color = { 0, 0, 0 };
	for (auto& light : lights)
	{
		// TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
		// components are. Then, accumulate that result on the *result_color* object.
		float r2 = (light.position - point).dot((light.position - point));
		Eigen::Vector3f l = (light.position - point).normalized();
		Eigen::Vector3f n = normal.normalized();
		Eigen::Vector3f v = (eye_pos - point).normalized();
		Eigen::Vector3f h = (l + v).normalized();
		Eigen::Vector3f ambient = ka * amb_light_intensity[0];
		Eigen::Vector3f diffuse = (kd * light.intensity[0] / r2) * std::max(0.0f, n.dot(l));
		Eigen::Vector3f specular = (ks * light.intensity[0] / r2) * std::pow(std::max(0.f, (n.dot(h))), p);
		
		if (diffuse[0] > 0.0||diffuse[1]>0.0||diffuse[2]>0.0) {
			int a = 10;
		}

		result_color += (ambient + diffuse + specular);

	}

	return result_color ;
}

static bool insideTriangle(int x, int y, const Vector4f* _v) {
	Vector3f v[3];
	for (int i = 0; i < 3; i++)
		v[i] = { _v[i].x(),_v[i].y(), 1.0 };
	Vector3f f0, f1, f2;
	f0 = v[1].cross(v[0]);
	f1 = v[2].cross(v[1]);
	f2 = v[0].cross(v[2]);
	Vector3f p(x, y, 1.);
	if ((p.dot(f0) * f0.dot(v[2]) > 0) && (p.dot(f1) * f1.dot(v[0]) > 0) && (p.dot(f2) * f2.dot(v[1]) > 0))
		return true;
	return false;
}
bool isPointInTriangle(double x1, double y1,
	double x2, double y2,
	double x3, double y3,
	double x, double y) {
	// ???? v1 = (x2-x1, y2-y1), v2 = (x3-x1, y3-y1), p = (x-x1, y-y1)
	double v1x = x2 - x1, v1y = y2 - y1; // ?? AB
	double v2x = x3 - x1, v2y = y3 - y1; // ?? AC
	double px = x - x1, py = y - y1;     // ?? AP

	// ??????????
	double cross_v1p = v1x * py - v1y * px; // AB ?AP
	double cross_v2p = v2x * py - v2y * px; // AC ?AP
	double cross_v1v2 = v1x * v2y - v1y * v2x; // AB ?AC

	// ?????0????????0?
	if (cross_v1v2 == 0) {
		// ????????????????false
		return false;
	}

	// ?????? u, v
	double u = cross_v1p / cross_v1v2;
	double v = cross_v2p / cross_v1v2;

	// ?????u >= 0, v >= 0, u + v <= 1
	return (u >= 0) && (v >= 0) && (u + v <= 1);
}
static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector4f* v) {
	float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
	float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
	float c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
	return { c1,c2,c3 };
}
RasterizationDevice::RasterizationDevice(const QSize& size) :m_ImageSize(size)
{

	ImageBuffer.push_back(new QImage(size, QImage::Format_ARGB32));
	ImageBuffer.push_back(new QImage(size, QImage::Format_ARGB32));

	for (int i = 0; i < 2; ++i) {
		ImageBuffer[i]->fill(Qt::white);
	}

	depth_buf.resize(m_ImageSize.width() * m_ImageSize.height());

	m_threadpool = std::make_unique<ThreadPool>(std::thread::hardware_concurrency());
	m_threadpool->Start();
}

RasterizationDevice::~RasterizationDevice()
{
	for (auto it : ImageBuffer) {
		if (it) {
			delete it;
		}
	}

}

QImage* RasterizationDevice::GetImage()
{
	return ImageBuffer[0];
}

void RasterizationDevice::SetViewPortSize(const QSize& size)
{
	m_ImageSize = size;
}

void RasterizationDevice::StartRending()
{
	ImageBuffer[1]->fill(Qt::gray);
	std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::max());
}


void RasterizationDevice::FinishRender()
{
	*ImageBuffer[0] = ImageBuffer[1]->copy();
}
void RasterizationDevice::SetImageSize(const QSize& size)
{
	if (size != m_ImageSize) {
		m_ImageSize = size;
	}

	for (int i = 0; i < 2; ++i) {
		*ImageBuffer[i]=ImageBuffer[i]->scaled(size);
	}
	depth_buf.resize(m_ImageSize.width() * m_ImageSize.height());
}
void RasterizationDevice::SetTriangles(const std::vector<std::shared_ptr<Triangle>>& triangles)
{
	m_triangles = triangles;
}
void RasterizationDevice::SetMVPTransformer(MVPTransformer* transformer)
{
	m_transformer = transformer;
}

void RasterizationDevice::Draw()
{
	float f1 = (50 - 0.1f) / 2.0f;
	float f2 = (50 + 0.1f) / 2.0f;

	auto projection = m_transformer->get_projection_matrix();
	auto view = m_transformer->get_view_matrix();
	auto model = m_transformer->get_model_matrix();
	Eigen::Matrix4f mvp = projection * view * model;
	Eigen::Matrix4f mv = view * model;
	Eigen::Matrix4f inv_trans = (mv).inverse().transpose();

	int ImageWidth = m_ImageSize.width();
	int ImageHeight = m_ImageSize.height();

	StartRending();

	auto now = std::chrono::high_resolution_clock().now();

	int m_triangles_size = m_triangles.size();
	std::vector<TriWithAABB> triwithAABB(m_triangles_size, {});

	for (int i = 0;i < m_triangles_size;++i)
	{
		auto t = m_triangles[i];
		Triangle newtri = *t;

		std::array<Eigen::Vector4f, 3> mm{
			(mv * t->v[0]),
			(mv * t->v[1]),
			(mv * t->v[2])
		};

		std::array<Eigen::Vector3f, 3> viewspace_pos;
		std::transform(mm.begin(), mm.end(), viewspace_pos.begin(),
			[](auto& v) {return v.template head<3>(); });

		Eigen::Vector4f v[] = {
			mvp * t->v[0],
			mvp * t->v[1],
			mvp * t->v[2]
		};
		// Homogeneous division
		for (auto& vec : v) {
			vec.x() /= vec.w();
			vec.y() /= vec.w();
			vec.z() /= vec.w();
		}

		Eigen::Vector4f n[] = {
			inv_trans * to_vec4(t->normal[0], 0.0f),
			inv_trans * to_vec4(t->normal[1], 0.0f),
			inv_trans * to_vec4(t->normal[2], 0.0f)
		};

		// Viewport transformation
		for (auto& vert : v) {
			vert.x() = 0.5f * ImageWidth * (vert.x() + 1.0f);
			vert.y() = 0.5f * ImageHeight * (vert.y() + 1.0f);
			vert.z() = vert.z() * f1 + f2;
		}

		for (int i = 0; i < 3; ++i)
			newtri.setVertex(i, v[i]);

		for (int i = 0; i < 3; ++i)
			newtri.setNormal(i, n[i].head<3>());

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
	

	const int BLOCK_ROWS = 8;
	std::vector<std::future<void>> futures;

	for (int y_begin = 0; y_begin < ImageHeight; y_begin += BLOCK_ROWS)
	{
		int y_end = std::min(y_begin + BLOCK_ROWS, ImageHeight);

		auto fut = m_threadpool->AddRetTask([this, y_begin, y_end, ImageWidth, ImageHeight,
			&triwithAABB]() mutable -> void
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
									m_textureImage ? &*m_textureImage : nullptr);
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


	auto delta = std::chrono::high_resolution_clock().now() - now;
	std::chrono::milliseconds seconds = std::chrono::duration_cast<std::chrono::milliseconds>(delta);
	std::cout << " one picture use " << seconds.count() <<" milliseconds" << std::endl;
	FinishRender();


}

void RasterizationDevice::SetTextureImage(TextureImage* textureImage)
{
	m_textureImage = textureImage;
}
void RasterizationDevice::set_pixel(const Eigen::Vector2i& point, const Eigen::Vector3f& color)
{
	int y = m_ImageSize.height()-point.y(); //计算的数学坐标系 原点是左下角  Qt 图片原点是左上角
	int x = point.x();
	if (x < 0 || x >= m_ImageSize.width() || y < 0 || y >= m_ImageSize.height())
		return;

	int r = std::clamp((int)(255.0f * color[0]), 0, 255);
	int g = std::clamp((int)(255.0f * color[1]), 0, 255);
	int b = std::clamp((int)(255.0f * color[2]), 0, 255);

	QRgb* line = reinterpret_cast<QRgb*>(ImageBuffer[1]->scanLine(y));
	line[x] = qRgb(r, g, b);
}
std::vector<double> RasterizationDevice::getScanIntersectX(const Triangle& tri, double y_scan)
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

void  RasterizationDevice::rasterize_triangle(const Triangle& t, const std::array<Eigen::Vector3f, 3>& view_pos)
{

	auto v = t.toVector4();
	int xmin = 0;
	int xmax = 0;
	int ymin = 0;
	int ymax = 0;
	float xminf = t.v[0].x();
	float xmaxf = t.v[0].x();
	float yminf = t.v[0].y();
	float ymaxf = t.v[0].y();

	for (int i = 0; i < 3; i++)
	{
		if (v[i].x() < xminf) xminf = t.v[i].x();
		if (v[i].x() > xmaxf) xmaxf = t.v[i].x();
		if (v[i].y() < yminf) yminf = t.v[i].y();
		if (v[i].y() > ymaxf) ymaxf = t.v[i].y();
	}
	xmin = xminf;
	xmax = xmaxf + 1;
	ymin = yminf;
	ymax = ymaxf + 1;

	for (int j = ymin; j < ymax; j++) {
		auto x_array = getScanIntersectX(t, j);
		if (x_array.size() <= 1) { continue; }
		int left = x_array[0] ;
		int right = x_array[1];


		for (int i = left;i <= right; ++i) {
			//Depth interpolated
			auto [alpha, beta, gamma] = computeBarycentric2D(i + 0.5, j + 0.5, t.v);

			float Z = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
			float zp = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
			zp *= Z;
			float temp = depth_buf[get_index(i, j)];
			if (zp < depth_buf[get_index(i, j)])
			{
				depth_buf[get_index(i, j)] = zp;
				auto interpolated_color = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], 1);
				auto interpolated_normal = interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], 1).normalized();
				auto a1 = t.tex_coords[0];
				auto a2 = t.tex_coords[1];
				auto a3 = t.tex_coords[2];
				auto interpolated_texcoords = alpha * a1 + beta * a2 + gamma * a3;

				auto interpolated_shadingcoords = interpolate(alpha, beta, gamma, view_pos[0], view_pos[1], view_pos[2], 1);
				fragment_shader_payload payload(interpolated_color, interpolated_normal, interpolated_texcoords, m_textureImage ? &*m_textureImage : nullptr);
				payload.view_pos = interpolated_shadingcoords;


				auto pixel_color = texture_fragment_shader(payload);
				set_pixel(Eigen::Vector2i(i, j), pixel_color);
			}

		}
	}


	//for (int i = xmin; i < xmax; i++)
	//{
	//	for (int j = ymin; j < ymax; j++)
	//	{
	//		if (insideTriangle(i + 0.5, j + 0.5, t.v))
	//		{
	//			//Depth interpolated
	//			auto [alpha, beta, gamma] = computeBarycentric2D(i + 0.5, j + 0.5, t.v);

	//			float Z = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
	//			float zp = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
	//			zp *= Z;
	//			float temp = depth_buf[get_index(i, j)];
	//			if (zp < depth_buf[get_index(i, j)])
	//			{
	//				depth_buf[get_index(i, j)] = zp;
	//				auto interpolated_color = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], 1);
	//				auto interpolated_normal = interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], 1).normalized();
	//				auto a1 =t.tex_coords[0];
	//				auto a2 = t.tex_coords[1];
	//				auto a3 = t.tex_coords[2];
	//				auto interpolated_texcoords = alpha* a1 + beta * a2 + gamma * a3;
	//			
	//				auto interpolated_shadingcoords = interpolate(alpha, beta, gamma, view_pos[0], view_pos[1], view_pos[2], 1);
	//				fragment_shader_payload payload(interpolated_color, interpolated_normal, interpolated_texcoords, m_textureImage? &*m_textureImage: nullptr);
	//				payload.view_pos = interpolated_shadingcoords;

	//			
	//				auto pixel_color = texture_fragment_shader(payload);
	//				set_pixel(Eigen::Vector2i(i, j),pixel_color);
	//			}
	//		}
	//	}

	//}

}
int RasterizationDevice::get_index(int x, int y)
{
	return ( m_ImageSize.height() - y) *  m_ImageSize.width() + x;
}
