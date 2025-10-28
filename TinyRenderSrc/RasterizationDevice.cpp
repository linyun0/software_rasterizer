#include "RasterizationDevice.h"
#include <QPainter>
#include "Triangle.h"
#include "MVPTransformer.h"
#include <array>
#include "Shader.h"
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
	SetViewPort();

	ImageBuffer.push_back(new QImage(size, QImage::Format_ARGB32));
	ImageBuffer.push_back(new QImage(size, QImage::Format_ARGB32));
	ImageBuffer.push_back(new QImage(size, QImage::Format_ARGB32));

	for (int i = 0; i < 2; ++i) {
		ImageBuffer[i]->fill(Qt::white);
	}

	m_lightPosition=glm::vec3( 20,20,20 );
	//m_lightPosition=glm::vec3( 0,1,0 );
	depth_buf.resize(m_ImageSize.width() * m_ImageSize.height());

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
	SetViewPort();
}

void RasterizationDevice::StartRending()
{
	ImageBuffer[1]->fill(Qt::black);
}
double doublemin(const double& a, const double& b) {
	return a < b ? a : b;
}
double doublemax(const double& a, const double& b) {
	return a > b ? a : b;
}

std::vector<int> RasterizationDevice::GetBoundingBox(const std::vector<glm::vec3>& triangle)
{
	if (triangle.size() < 1) {
		return std::vector<int>();
	}
	double ax = triangle[0].x;
	double ay = triangle[0].y;
	double bx = triangle[1].x;
	double by = triangle[1].y;
	double cx = triangle[2].x;
	double cy = triangle[2].y;

	int xmin = INT_MAX, ymin = INT_MAX, xmax = INT_MIN, ymax = INT_MIN;


	xmin = std::round(doublemin(doublemin(ax, bx), cx));  //topLeft 
	ymin = std::round(doublemin(doublemin(ay, by), cy));
	xmax = std::round(doublemax(doublemax(ax, bx), cx));  //bottomRight
	ymax = std::round(doublemax(doublemax(ay, by), cy));

	return std::vector<int>{xmin, ymin, xmax, ymax};
}
std::vector<std::vector<int>>  RasterizationDevice::ScalLine(const std::vector<glm::vec3>& triangle)
{
	double min_y_val = INT_MAX;
	double max_y_val = INT_MIN;
	std::vector<std::vector<int>> ans;

	int y_min_index = 0, y_max_index = 0;
	for (int i = 0; i < 3; ++i) {
		if (triangle[i].y < min_y_val) {
			min_y_val = triangle[i].y;
			y_min_index = i;
		}
		if (triangle[i].y > max_y_val) {
			max_y_val = triangle[i].y;
			y_max_index = i;
		}
	}
	if (y_min_index == y_max_index) {
		return ans;
	}

	for (int i = 0; i < 3; ++i) {
		if (i == y_min_index) {
			continue;
		}
		if (triangle[i].y == min_y_val) {

			double leftx, lefty, rightx, righty;
			if (triangle[i].x < triangle[y_min_index].x) {
				leftx = triangle[i].x;
				lefty = triangle[i].y;
				rightx = triangle[y_min_index].x;
				righty = triangle[y_min_index].y;
			}
			else {
				leftx = triangle[y_min_index].x;
				lefty = triangle[y_min_index].y;
				rightx = triangle[i].x;
				righty = triangle[i].y;
			}

			double upx = triangle[y_max_index].x, upy = triangle[y_max_index].y;

			for (int y = min_y_val; y <= max_y_val; ++y) {

				double t1 = (y - lefty) / (upy - lefty);
				double xmin = leftx + t1 * (upx - leftx);

				double t2 = (y - righty) / (upy - righty);
				double xmax = rightx + t1 * (upx - rightx);

				std::vector<int> temp{ y,int(xmin),int(xmax) };
				ans.push_back(temp);
			}

			return ans;
		}
	}

	for (int i = 0; i < 3; ++i) {
		if (i == y_max_index) {
			continue;
		}
		if (triangle[i].y == max_y_val) {

			double leftx, lefty, rightx, righty;
			if (triangle[i].x < triangle[y_max_index].x) {
				leftx = triangle[i].x;
				lefty = triangle[i].y;
				rightx = triangle[y_max_index].x;
				righty = triangle[y_max_index].y;
			}
			else {
				leftx = triangle[y_max_index].x;
				lefty = triangle[y_max_index].y;
				rightx = triangle[i].x;
				righty = triangle[i].y;
			}

			double downx = triangle[y_min_index].x, downy = triangle[y_min_index].y;

			for (int y = min_y_val; y <= max_y_val; ++y) {

				double t1 = (y - lefty) / (downy - lefty);
				double xmin = leftx + t1 * (downx - leftx);

				double t2 = (y - righty) / (downy - righty);
				double xmax = rightx + t1 * (downx - rightx);

				std::vector<int> temp{ y,int(xmin),int(xmax) };
				ans.push_back(temp);
			}

			return ans;
		}
	}

	for (int i = 0; i < 3; ++i) {

		if (i != y_min_index && i != y_max_index)
		{


			for (int y = min_y_val; y <= triangle[i].y; ++y) {

				double t1 = (y - triangle[y_min_index].y) / (triangle[y_max_index].y - triangle[y_min_index].y);
				double x1 = triangle[y_min_index].x + t1 * (triangle[y_max_index].x - triangle[y_min_index].x);

				double t2 = (y - triangle[i].y) / (triangle[y_min_index].y - triangle[i].y);
				double x2 = triangle[i].x + t1 * (triangle[y_min_index].x - triangle[i].x);

				double xmin = x1, xmax = x2;
				if (x2 < x1) {
					double temp = xmin;
					xmin = xmax;
					xmax = temp;
				}

				std::vector<int> temp{ y,int(xmin),int(xmax) };
				ans.push_back(temp);
			}

			for (int y = triangle[i].y + 1; y <= triangle[y_max_index].y; ++y) {

				double t1 = (y - triangle[y_min_index].y) / (triangle[y_max_index].y - triangle[y_min_index].y);
				double x1 = triangle[y_min_index].x + t1 * (triangle[y_max_index].x - triangle[y_min_index].x);

				double t2 = (y - triangle[i].y) / (triangle[y_max_index].y - triangle[i].y);
				double x2 = triangle[i].x + t1 * (triangle[y_max_index].x - triangle[i].x);

				double xmin = x1, xmax = x2;
				if (x2 < x1) {
					double temp = xmin;
					xmin = xmax;
					xmax = temp;
				}

				std::vector<int> temp{ y,int(xmin),int(xmax) };
				ans.push_back(temp);
			}

			return ans;


		}

	}


	return ans;

}
bool RasterizationDevice::isInTriangle(const std::vector<int>& point, const std::vector<glm::vec3>& triangel)
{
	double x1 = triangel[0].x;
	double y1 = triangel[0].y;
	double x2 = triangel[1].x;
	double y2 = triangel[1].y;
	double x3 = triangel[2].x;
	double y3 = triangel[2].y;


	int v1x = x2 - x1;
	int v1y = y2 - y1;
	int p1x = point[0] - x1;
	int p1y = point[1] - y1;

	bool is = false;
	is = crossProduct(p1x, p1y, v1x, v1y);

	int v2x = x3 - x2;
	int v2y = y3 - y2;
	int p2x = point[0] - x2;
	int p2y = point[1] - y2;
	if (is != crossProduct(p2x, p2y, v2x, v2y))
	{
		return false;
	};

	int v3x = x1 - x3;
	int v3y = y1 - y3;
	int p3x = point[0] - x3;
	int p3y = point[1] - y3;
	if (is != crossProduct(p3x, p3y, v3x, v3y))
	{
		return false;
	};

	return true;


}

void RasterizationDevice::SetLightDirection(const glm::vec3& lightDirection)
{
	m_lightPosition= lightDirection;
}

bool RasterizationDevice::crossProduct(const double& x1, const double& y1, const double& x2, const double& y2)
{
	return (x1 * y2 - x2 * y1) > 0 ? true : false;
}




std::vector<double> RasterizationDevice::baryCentric(const std::vector<int>& point, const std::vector <glm::vec3> & triangle)
{
	// 使用double避免整数溢出
	double ax = triangle[0].x, ay = triangle[0].y;
	double bx = triangle[1].x, by = triangle[1].y;
	double cx = triangle[2].x, cy = triangle[2].y;
	double px_d = point[0], py_d = point[1];

	// 计算边向量
	double abx = bx - ax;
	double aby = by - ay;
	double acx = cx - ax;
	double acy = cy - ay;
	double apx = px_d - ax;
	double apy = py_d - ay;

	// 计算叉积 (有向面积)
	double SABC = abx * acy - aby * acx;
	double SABP = abx * apy - aby * apx;  // AB × AP (对应gamma)
	double SAPC = apx * acy - apy * acx;   // AP × AC (对应beta)

	// 处理退化三角形 (使用绝对值和容差)
	if (std::abs(SABC) < 1e-5) {
		return { 0.0, 0.0, 0.0 };
	}

	// 正确计算重心坐标
	double beta = SAPC / SABC;      // 顶点B的权重
	double gamma = SABP / SABC;     // 顶点C的权重
	double alpha = 1.0 - beta - gamma; // 顶点A的权重

	return { alpha, beta, gamma };
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
	depth_buf.resize(m_ImageSize.width() * m_ImageSize.height());
}
void RasterizationDevice::SetTriangles(const std::vector<Triangle*>& triangles)
{
	m_triangles = triangles;
}
void RasterizationDevice::SetMVPTransformer(MVPTransformer* transformer)
{
	m_transformer = transformer;
}
void RasterizationDevice::Draw()
{
	float f1 = (50 - 0.1) / 2.0;
	float f2 = (50 + 0.1) / 2.0;
		
	auto projection = m_transformer->get_projection_matrix();
	auto view = m_transformer->get_view_matrix();
	auto model = m_transformer->get_model_matrix();
	Eigen::Matrix4f mvp = projection* view * model;

	for (const auto& t : m_triangles)
	{
		Triangle newtri = *t;

		std::array<Eigen::Vector4f, 3> mm{
			  (view * model * t->v[0]),
			  (view * model * t->v[1]),
			  (view * model * t->v[2])
		};

		std::array<Eigen::Vector3f, 3> viewspace_pos;

		std::transform(
			mm.begin(),
			mm.end(),
			viewspace_pos.begin(),
			[](auto& v) {return v.template head<3>(); }
		);

		Eigen::Vector4f v[] = {
				mvp * t->v[0],
				mvp * t->v[1],
				mvp * t->v[2]
		};
		//Homogeneous division
		for (auto& vec : v) {
			vec.x() /= vec.w();
			vec.y() /= vec.w();
			vec.z() /= vec.w();
		}

		Eigen::Matrix4f inv_trans = (view * model).inverse().transpose();
		Eigen::Vector4f n[] = {
				inv_trans * to_vec4(t->normal[0], 0.0f),
				inv_trans * to_vec4(t->normal[1], 0.0f),
				inv_trans * to_vec4(t->normal[2], 0.0f)
		};

		//Viewport transformation
		for (auto& vert : v)
		{
			vert.x() = 0.5 *m_ImageSize.width() * (vert.x() + 1.0);
			vert.y() = 0.5 * m_ImageSize.height() * (vert.y() + 1.0);
			vert.z() = vert.z() * f1 + f2;
		}

		for (int i = 0; i < 3; ++i)
		{
			//screen space coordinates
			newtri.setVertex(i, v[i]);
		}

		for (int i = 0; i < 3; ++i)
		{
			//view space normal
			//std::cout<<i<<':'<<n[i].head<3>()<<'\n';
			newtri.setNormal(i, n[i].head<3>());
		}

		newtri.setColor(0, 148, 121.0, 92.0);
		newtri.setColor(1, 148, 121.0, 92.0);
		newtri.setColor(2, 148, 121.0, 92.0);
		
		// Also pass view space vertice position
		rasterize_triangle(newtri, viewspace_pos);
	}
	//ImageBuffer[2]->save("testgames.png");
	FinishRender();

}
//void RasterizationDevice::set_pixel(const double& point, const double& color)
//{
//	Vector2i point1;
//	Eigen::Vector3f colo1r;
//}
void RasterizationDevice::testSave()
{
}
void RasterizationDevice::set_pixel(const Eigen::Vector2i& point, const Eigen::Vector3f& color)
{
	ImageBuffer[1]->setPixelColor(point.x(), point.y()
		,QColor(255.0*color[0], 255.0*color[1], 255.0*color[2]));

}
void  RasterizationDevice::rasterize_triangle(const Triangle& t, const std::array<Eigen::Vector3f, 3>& view_pos)
{
	//每个三角形数据结构：
//Vector4f v[3]; /*the original coordinates of the triangle, v0, v1, v2 in counter clockwise order*/
//Vector3f color[3]; //color at each vertex;
//Vector2f tex_coords[3]; //texture u,v
//Vector3f normal[3]; //normal vector for each vertex

//经过之前的处理后，传入三角形screen_space的坐标t.v以及viewspace的顶点坐标。
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

	// TODO: From your HW3, get the triangle rasterization code.
	// TODO: Inside your rasterization loop:
	//    * v[i].w() is the vertex view space depth value z.
	//    * Z is interpolated view space depth for the current pixel
	//    * zp is depth between zNear and zFar, used for z-buffer

	// float Z = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
	// float zp = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
	// zp *= Z;

	for (int i = xmin; i < xmax; i++)
	{
		for (int j = ymin; j < ymax; j++)
		{
			if (insideTriangle(i + 0.5, j + 0.5, t.v))
			{
				//Depth interpolated
				auto [alpha, beta, gamma] = computeBarycentric2D(i + 0.5, j + 0.5, t.v);

				float Z = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
				float zp = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
				zp *= Z;

				if (zp < depth_buf[get_index(i, j)])
				{
					depth_buf[get_index(i, j)] = zp;
					auto interpolated_color = interpolate(alpha, beta, gamma, t.color[0], t.color[1], t.color[2], 1);
					auto interpolated_normal = interpolate(alpha, beta, gamma, t.normal[0], t.normal[1], t.normal[2], 1).normalized();
				//	auto interpolated_texcoords = interpolate(alpha, beta, gamma, t.tex_coords[0], t.tex_coords[1], t.tex_coords[2], 1);
					auto interpolated_shadingcoords = interpolate(alpha, beta, gamma, view_pos[0], view_pos[1], view_pos[2], 1);
					fragment_shader_payload payload(interpolated_color, interpolated_normal/*, interpolated_texcoords, texture ? &*texture : nullptr*/);
					payload.view_pos = interpolated_shadingcoords;

					auto pixel_color = phong_fragment_shader(payload);
					set_pixel(Eigen::Vector2i(i, j),pixel_color);
				}
			}

		}
	}

}
int RasterizationDevice::get_index(int x, int y)
{
	return ( m_ImageSize.height() - y) *  m_ImageSize.width() + x;
}
std::vector<glm::vec3>  RasterizationDevice::ViewPortTransform(glm::mat4x3  m_triangleMatrix)
{
	auto vec = m_triangleMatrix[3];
	glm::mat4x3 triangle = m_triangleMatrix;
	//auto triangle = m_triangleMatrix / vec;
	for (int i = 0; i < 3; ++i) {
		triangle[i] = triangle[i] / vec;
	}
	//*m_viewport* triangle;
	triangle =triangle* (*m_viewport);
	std::vector<glm::vec3> points;
	for (int j = 0; j < 3; ++j) {
		glm::vec3 temp{ triangle[0][j],triangle[1][j],triangle[2][j] };
		points.push_back(temp);
	}

	return points;
}
double calculateDistance(glm::vec3& pointLocation,
	glm::vec3 lightLocation) {
	double dx = pointLocation.x - lightLocation.x;
	double dy = pointLocation.y - lightLocation.y;
	double dz = pointLocation.z - lightLocation.z;
	return sqrt(dx * dx + dy * dy + dz * dz); // ????????
}

//
//void RasterizationDevice::rendingTriangle(const triangle& singletriangle, const bool& isframe)
//{
//	auto triangle = ViewPortTransform(singletriangle.m_triangleMatrix);
//	auto normals = singletriangle.m_normals;
//	auto uv = singletriangle.m_uv;
//	if (isframe) {
//		mtx.lock();
//		QPainter painter(ImageBuffer[1]);
//
//		painter.setPen(Qt::red);
//
//		painter.drawLine(triangle[0].x, triangle[0].y, triangle[1].x, triangle[1].y);
//		painter.drawLine(triangle[1].x, triangle[1].y, triangle[2].x, triangle[2].y);
//		painter.drawLine(triangle[2].x, triangle[2].y, triangle[0].x, triangle[0].y);
//
//		painter.end();
//		mtx.unlock();
//	}
//	else
//	{
//
//
//		std::vector<int> BoundingBox = GetBoundingBox(triangle);
//#pragma omp parallel for
//		for (int x = BoundingBox[0]; x <= BoundingBox[2]; ++x) {
//
//			bool isIn = false;
//#pragma omp parallel for
//			for (int y = BoundingBox[1]; y <= BoundingBox[3]; ++y) {
//
//				std::vector<double> args = baryCentrictemp(triangle[0].x, triangle[0].y, triangle[1].x, triangle[1].y, triangle[2].x, triangle[2].y, x, y);
//				if (args[0] < 0 || args[1] < 0 || args[2] < 0) {
//					if (isIn) {
//						break;
//					}
//					continue;
//				};
//				isIn = true;
//				double pointZ = args[0] * triangle[0].z +
//					args[1] * triangle[1].z +
//					args[2] * triangle[2].z;
//				if (pointZ < zBuffer[x][y]) {
//					continue;
//				}
//				QSize size = ImageBuffer[1]->size();
//				if (x < 0 || x >= size.width() || y < 0 || y >= size.height()) {
//					continue;
//				}
//
//				mtx.lock();
//				zBuffer[x][y] = pointZ;
//				mtx.unlock();
//				//light
//				auto matrix = singletriangle.m_triangleMatrix;
//				auto vecz = matrix[3];
//				matrix = matrix / vecz;
//
//				glm::vec3 the3dPoint1{ matrix[0][0],matrix[1][0],matrix[2][0] };
//				glm::vec3 the3dPoint2{ matrix[0][1],matrix[1][1],matrix[2][1] };
//				glm::vec3 the3dPoint3{ matrix[0][2],matrix[1][2],matrix[2][2] };
//				glm::vec3 currentPoint = args[0] * the3dPoint1 + args[1] * the3dPoint2 + args[2] * the3dPoint3;
//				double r = calculateDistance(currentPoint, glm::vec3{ m_lightDirection[0],m_lightDirection[1],m_lightDirection[2] });
//
//				glm::vec3 newNormal = normals[0] * args[0] + normals[1] * args[1] + normals[2] * args[2];
//				double intensity = m_lightDirection[0] * newNormal[0] +
//					m_lightDirection[1] * newNormal[1] +
//					m_lightDirection[2] * newNormal[2];
//				intensity = std::max(0.0, std::min(intensity, 1.0));
//				if (std::abs(intensity - 0) == 0) {
//					continue;
//				}
//				double newintensity = (1 / (r * r)) * intensity;
//				intensity = newintensity * 3;
//				// intensity = (1/(r*r))*intensity;
//
//				vec2 realuv = uv[0] * args[0] + uv[1] * args[1] + uv[2] * args[2];
//				QColor color;
//				if (m_texture)
//				{
//					int width = m_texture->width();
//					int height = m_texture->height();
//
//					int texX = realuv.x * (m_texture->width() - 1);
//					int texY = realuv.y * (m_texture->height() - 1);
//
//
//					QColor  tempcolor = m_texture->pixelColor(texX, texY);
//					auto r = tempcolor.red();
//					auto g = tempcolor.green();
//					auto b = tempcolor.blue();
//					auto a = tempcolor.alpha();
//					// ? ?  ?
//					auto tempr = tempcolor.red();
//
//					color = QColor(r * intensity,
//						g * intensity,
//						b * intensity, a);
//
//					if (color.red() == 255 || color.green() == 255 || color.blue() == 255) {
//						int a = 10;
//					}
//				}
//				else {
//					color = QColor(intensity * 255,
//						intensity * 255,
//						intensity * 255, 255);
//				}
//
//
//				//	QColor color = QColor(intensity * 255, intensity * 255, intensity * 255, 255);
//
//				mtx.lock();
//				ImageBuffer[1]->setPixelColor(x, y
//					, color);
//				mtx.unlock();
//			}
//		}
//	}
//
//}
//
//
//void RasterizationDevice::rendingTriangles(const std::vector<triangle>& triangles, const QImage* texture)
//{
//	StartRending();
//	int size = triangles.size();
//	m_texture = texture;
//
//	for (int i = 0; i < size; ++i)
//	{
//		rendingTriangle(triangles[i], false);
//	}
//	FinishRender();
//}
//



//void RasterizationDevice::RenderPointsImage(const std::vector<Vertex>& arrayVertex, const QColor& color)
//{
//	StartRending();
//	int size = arrayVertex.size();
//	for (int i = 0; i < size; ++i) {
//		mat<4, 1> pos{ arrayVertex[i].Position.x,arrayVertex[i].Position.y ,arrayVertex[i].Position.z ,1 };
//		auto drawPos = *(m_viewport)*pos;
//		ImageBuffer[1]->setPixelColor(drawPos[0][0], drawPos[1][0]
//								, color);
//
//	}
//	ImageBuffer[1]->save("test.png");
//	FinishRender();
//
//}
//

