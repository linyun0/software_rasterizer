#include "RasterizationDevice.h"
#include <QPainter>
//TGAColor sample2D(const TGAImage& img, const vec2& uvf) {
//	return img.get(uvf[0] * img.width(), uvf[1] * img.height());
//}

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
RasterizationDevice::RasterizationDevice(const QSize& size) :m_ImageSize(size)
{
	SetViewPort();

	ImageBuffer.push_back(new QImage(size, QImage::Format_ARGB32));
	ImageBuffer.push_back(new QImage(size, QImage::Format_ARGB32));

	for (int i = 0; i < 2; ++i) {
		ImageBuffer[i]->fill(Qt::white);
	}

	m_lightDirection = std::vector<double>{ 0,0,1 };

	//void DrawALine::viewport(const int x, const int y, const int w, const int h)
	//{
	//	m_viewPortMatrix = { {{w / 2., 0, 0, x + w / 2.}, {0, h / 2., 0, y + h / 2.}, {0,0,1,0}, {0,0,0,1}} };
	//}

 //   m_view= { {{w / 2., 0, 0, x + w / 2.}, {0, h / 2., 0, y + h / 2.}, {0,0,1,0}, {0,0,0,1}} };

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
	ImageBuffer[1]->fill(Qt::white);
}
double doublemin(const double& a, const double& b) {
	return a < b ? a : b;
}
double doublemax(const double& a, const double& b) {
	return a > b ? a : b;
}

std::vector<int> RasterizationDevice::GetBoundingBox(const std::vector<vec3>& triangle)
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
std::vector<std::vector<int>>  RasterizationDevice::ScalLine(const std::vector<vec3>& triangle)
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
bool RasterizationDevice::isInTriangle(const std::vector<int>& point, const std::vector<vec3>& triangel)
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

void RasterizationDevice::SetLightDirection(const std::vector<double>& lightDirection)
{
	if (lightDirection.size() < 1) {
		return;
	}
	m_lightDirection = lightDirection;
}

bool RasterizationDevice::crossProduct(const double& x1, const double& y1, const double& x2, const double& y2)
{
	return (x1 * y2 - x2 * y1) > 0 ? true : false;
}




std::vector<double> RasterizationDevice::baryCentric(const std::vector<int>& point, const std::vector<vec3>& triangle)
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
void RasterizationDevice::testSave()
{
	ImageBuffer[1]->save("testRending.png");
}
std::vector<vec3>  RasterizationDevice::ViewPortTransform(mat<4, 3>  m_triangleMatrix)
{
	auto vec = m_triangleMatrix[3];
	auto triangle = m_triangleMatrix / vec;
	triangle = *m_viewport * triangle;
	std::vector<vec3> points;
	for (int j = 0; j < 3; ++j) {
		vec3 temp{ triangle[0][j],triangle[1][j],triangle[2][j] };
		points.push_back(temp);
	}

	return points;
}
double calculateDistance(vec3& pointLocation,
	vec3 lightLocation) {
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
//				vec3 the3dPoint1{ matrix[0][0],matrix[1][0],matrix[2][0] };
//				vec3 the3dPoint2{ matrix[0][1],matrix[1][1],matrix[2][1] };
//				vec3 the3dPoint3{ matrix[0][2],matrix[1][2],matrix[2][2] };
//				vec3 currentPoint = args[0] * the3dPoint1 + args[1] * the3dPoint2 + args[2] * the3dPoint3;
//				double r = calculateDistance(currentPoint, vec3{ m_lightDirection[0],m_lightDirection[1],m_lightDirection[2] });
//
//				vec3 newNormal = normals[0] * args[0] + normals[1] * args[1] + normals[2] * args[2];
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

