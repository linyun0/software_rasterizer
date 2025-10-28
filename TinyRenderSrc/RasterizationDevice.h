#pragma once
#include <queue>
#include <vector>
#include <qimage.h>
#include <qsize.h>
#include <thread>
#include <vector>
#include <mutex>
#include <qcolor.h>
#include <atomic>
#include "geometry.h"
#include <qpainter.h>
#include <chrono>
#include <Eigen/Eigen>
class Triangle;
class MVPTransformer;
//Vector2i point1;
//Eigen::Vector3f colo1r;
//class Vector2i;
//namespace Eigen {
//	class Vector3f;
//}
class RasterizationDevice
{

public:
	RasterizationDevice(const QSize& size);
	~RasterizationDevice();
	QImage* GetImage();
	void SetViewPortSize(const QSize& size);
	void StartRending();
	std::vector<int> GetBoundingBox(const std::vector<glm::vec3>& triangle);
	std::vector<std::vector<int>> ScalLine(const std::vector<glm::vec3>& triangle);
	bool isInTriangle(const std::vector<int>& point, const std::vector<glm::vec3>& triangel);
	void SetLightDirection(const glm::vec3& lightDirection);
	bool crossProduct(const double& x1, const  double& y1, const double& x2, const double& y2);
	std::vector<double> baryCentric(const std::vector<int>& point, const std::vector<glm::vec3>& triangle);
	void FinishRender();
	void SetImageSize(const QSize& size);
	void RenderPointsImage(const std::vector<Vertex>& arrayVertex,const QColor& color=QColor(255,0,0))
	{
		StartRending();
		int size = arrayVertex.size();
		for (int i = 0; i < size; ++i) {
			glm::vec4 pos{ arrayVertex[i].mvpPosition.x,arrayVertex[i].mvpPosition.y ,arrayVertex[i].mvpPosition.z ,1 };
			auto drawPos =pos*( *(m_viewport));
			ImageBuffer[1]->setPixelColor(drawPos.x, drawPos.y
				, color);

		}
		ImageBuffer[1]->save("test.png");
		FinishRender();

	}
	void RenderWireFrameImage(const std::vector<Vertex>& arrayVertex,const QColor& color=QColor(255,0,0))
	{

		StartRending();
		int size = arrayVertex.size();
		QPainter painter(ImageBuffer[1]);
		painter.setPen(QPen(color, 2));
		for (int i = 0; i <= size-2; i+=3) {
			//glm::vec4 pos{ arrayVertex[i].mvpPosition.x,arrayVertex[i].mvpPosition.y ,arrayVertex[i].mvpPosition.z ,1 };
			glm::vec4 pos = glm::vec4{ arrayVertex[i].mvpPosition,1.0f };
			auto drawPos = pos*(*(m_viewport));
			//glm::vec4 pos1{ arrayVertex[i+1].mvpPosition.x,arrayVertex[i+1].mvpPosition.y ,arrayVertex[i+1].mvpPosition.z ,1 };
			glm::vec4 pos1 = glm::vec4{ arrayVertex[i+1].mvpPosition,1.0f };
			auto drawPos1 =pos1*( *(m_viewport));
			//glm::vec4 pos2{ arrayVertex[i+2].mvpPosition.x,arrayVertex[i+2].mvpPosition.y ,arrayVertex[i+2].mvpPosition.z ,1 };
			glm::vec4 pos2 = glm::vec4{ arrayVertex[i+2].mvpPosition,1.0f };
			auto drawPos2 =pos2*( *(m_viewport));
			
			painter.drawLine(drawPos.x,drawPos.y, drawPos1.x,drawPos1.y);
			painter.drawLine(drawPos1.x,drawPos1.y, drawPos2.x,drawPos2.y);
			painter.drawLine(drawPos2.x , drawPos2.y, drawPos.x, drawPos.y);

		}
		painter.end();
		ImageBuffer[1]->save("test.png");
		FinishRender();

	}
	void RenderImage(const std::vector<Vertex>& arrayVertex, const QColor& color = QColor(255, 0, 0))
	{
		auto start = std::chrono::high_resolution_clock::now();
		StartRending();
		int size = arrayVertex.size();
		int circleNumbers = 0;
		std::chrono::microseconds testseconds(0);
		for (int i = 0; i <= size - 2; i += 3) {
			glm::vec4 pos = glm::vec4{ arrayVertex[i].mvpPosition,1.0f };
			pos.y = pos.y;
			pos.x = pos.x;
			pos.z = pos.z;
			auto drawPos = pos * (*(m_viewport));
			glm::vec4 pos1 = glm::vec4{ arrayVertex[i + 1].mvpPosition,1.0f };
			pos1.y = pos1.y;
			pos1.x = pos1.x;
			pos1.z = pos1.z;
			auto drawPos1 = pos1 * (*(m_viewport));
			glm::vec4 pos2 = glm::vec4{ arrayVertex[i + 2].mvpPosition,1.0f };
			pos2.y = pos2.y;
			pos2.x = pos2.x;
			pos2.z = pos2.z;
			auto drawPos2 = pos2 * (*(m_viewport));

			glm::vec3 point1 = glm::vec3(drawPos);
			glm::vec3 point2 = glm::vec3(drawPos1);
			glm::vec3 point3 = glm::vec3(drawPos2);
			std::vector<glm::vec3> pointarray{ point1,point2,point3 };
			
			std::vector<int> BoundingBox= GetBoundingBox(pointarray);
			//#pragma omp parallel for
			glm::vec3 mvpoint1 = arrayVertex[i].mvPosition;
			glm::vec3 mvpoint2 = arrayVertex[i+1].mvPosition;
			glm::vec3 mvpoint3 = arrayVertex[i+2].mvPosition;
			glm::vec3 oripoint1 = arrayVertex[i].Position;
			glm::vec3 oripoint2 = arrayVertex[i+1].Position;
			glm::vec3 oripoint3 = arrayVertex[i+2].Position;
			glm::vec3 mvppoint1 = arrayVertex[i].mvpPosition;
			glm::vec3 mvppoint2 = arrayVertex[i+1].mvpPosition;
			glm::vec3 mvppoint3 = arrayVertex[i+2].mvpPosition;
			auto mvnormal1 = arrayVertex[i].mvNormal;
			auto mvnormal2 = arrayVertex[i+1].mvNormal;
			auto mvnormal3 = arrayVertex[i+2].mvNormal;
			auto orinormal1 = arrayVertex[i].Normal;
			auto orinormal2 = arrayVertex[i+1].Normal;
			auto orinormal3 = arrayVertex[i+2].Normal;

			auto uv1 = arrayVertex[i].TexCoords;
			auto uv2 = arrayVertex[i+1].TexCoords;
			auto uv3 = arrayVertex[i+2].TexCoords;
		for (int x = BoundingBox[0]; x <= BoundingBox[2]; ++x) {
			for (int y = BoundingBox[1]; y <= BoundingBox[3]; ++y) {
				++circleNumbers;
				QSize size = ImageBuffer[1]->size();
				if (x < 0 || x >= size.width() || y < 0 || y >= size.height()) {
					continue;
				}
				std::vector<float> args = baryCentrictemp(point1.x, point1.y, point2.x, point2.y, point3.x, point3.y, x, y);
				if (args[0] < 0 || args[1] < 0 || args[2] < 0) {
					continue;
				};
				//double pointZ = args[0] *mvppoint1.z +
				//	args[1] * mvppoint2.z +
				//	args[2] * mvppoint3.z;
				double pointZ = args[0] *oripoint1.z + args[1] *oripoint2.z + args[2] *oripoint3.z;
				if (pointZ < zBuffer[x][y]) {
					continue;
				}
				zBuffer[x][y] = pointZ;
				
				auto start = std::chrono::high_resolution_clock::now();
				//light
				glm::vec3 currentPoint = args[0] * mvpoint1+ args[1] * mvpoint2+ args[2] *mvpoint3;
				glm::vec3 oricurrentPoint = args[0] * oripoint1+ args[1] * oripoint2+ args[2] *oripoint3;
			//	double r = calculateDistance(currentPoint, glm::vec3{ m_lightDirection[0],m_lightDirection[1],m_lightDirection[2] });

				glm::vec3 newNormal = mvnormal1 * args[0] + mvnormal2 * args[1] + mvnormal3 * args[2];
				glm::vec3 orinewNormal = orinormal1 * args[0] + orinormal2 * args[1] + orinormal3 * args[2];
				double oriintensity = glm::dot(glm::normalize(m_lightPosition-oricurrentPoint), glm::normalize(orinewNormal));
				double intensity = glm::dot(glm::normalize(m_lightPosition-currentPoint), glm::normalize(newNormal));
	
				if (oriintensity > 0) {
					int a = 10;
				}

				if (intensity > 0) {
					int a = 10;
				}
				oriintensity = std::max(0.0, std::min(oriintensity, 1.0));
				oriintensity += 0.05;
				intensity = std::max(0.0, std::min(intensity, 1.0));
				intensity += 0.05;
				glm::vec2 realuv = uv1 * args[0] + uv2 * args[1] + uv3 * args[2];
				QColor color;
				if (m_texture)
				{
					int width = m_texture->width();
					int height = m_texture->height();

					int texX = realuv.x * (m_texture->width() - 1);
					int texY = realuv.y * (m_texture->height() - 1);


					QColor  tempcolor = m_texture->pixelColor(texX, texY);
					auto r = tempcolor.red();
					auto g = tempcolor.green();
					auto b = tempcolor.blue();
					auto a = tempcolor.alpha();
					// ? ?  ?
					auto tempr = tempcolor.red();

					color = QColor(r * intensity,
						g * intensity,
						b * intensity, a);

					if (color.red() == 255 || color.green() == 255 || color.blue() == 255) {
						int a = 10;
					}
				}
				else {
					//color = QColor(intensity * 255,
					//	intensity * 255,
					//	intensity * 255, 255);
					color = QColor(oriintensity * 255,
						oriintensity * 255,
						oriintensity * 255, 255);
				}

			//	color = QColor(122, 88, 66);
				ImageBuffer[1]->setPixelColor(x, y
					, color);
				auto end= std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
				testseconds+= duration;
			}
		}

			
		}
		FinishRender();

		// 记录结束时间
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		
		ImageBuffer[0]->save("tes0.png");
		ImageBuffer[1]->save("test_nz.png");
	}
	void RenderTriangle()
	{
	}
//	void rendingTriangles(const std::vector<triangle>& triangles, const QImage* image = nullptr);
//	void rendingTriangle(const triangle& singletriangle, const bool& isframe = false);
	std::vector<glm::vec3> ViewPortTransform(glm::mat4x3 m_triangleMatrix);


	std::vector<float> baryCentrictemp(const int& x1, const int& y1, const int& x2, const int& y2, const int& x3, const int& y3, const int& px, const int& py)
	{
		float ax = x1, ay = y1;
		float bx = x2, by = y2;
		float cx = x3, cy = y3;
		float px_d = px, py_d = py;

		float abx = bx - ax;
		float aby = by - ay;
		float acx = cx - ax;
		float acy = cy - ay;
		float apx = px_d - ax;
		float apy = py_d - ay;

		float SABC = abx * acy - aby * acx;
		float SABP = abx * apy - aby * apx;  // AB × AP (对应gamma)
		float SAPC = apx * acy - apy * acx;   // AP × AC (对应beta)

		if (std::abs(SABC) < 1e-5) {
			return { 0.0, 0.0, 0.0 };
		}

		float beta = SAPC / SABC;      // 顶点B的权重
		float gamma = SABP / SABC;     // 顶点C的权重
		float alpha = 1.0 - beta - gamma; // 顶点A的权重

		return { alpha, beta, gamma };
	}

	glm::mat4x4 GetViewPort()
	{
		return *m_viewport;
	}
	void SetTriangles(const std::vector<Triangle*>& triangles);
	void SetMVPTransformer(MVPTransformer* transformer);
	void Draw();
private:
	void SetViewPort()
	{
		double x = m_ImageSize.width() / 8.0;
		double y = m_ImageSize.height() / 8.0;
		double w = 6 * x;
		double h = 6 * y;

		//w *= 4;
		//h *= 4;

		m_viewport = new glm::mat4x4{ {w / 2., 0, 0, x + w / 2.},
			{0, h / 2., 0, y + h / 2.},
			{0,0,1,0},
			{0,0,0,1}} ;
		zBuffer.clear();
		for (int i = 1; i < m_ImageSize.width() + 1; ++i) {
			zBuffer.push_back(std::vector<double>(m_ImageSize.height() + 1,INT_MIN));
		}

	}

	void testSave();
	void rasterize_triangle(const Triangle& t, const std::array<Eigen::Vector3f, 3>&  view_pos);
	//void set_pixel(const double& point, const double& color);
	void set_pixel(const Eigen::Vector2i& point, const Eigen::Vector3f& color);
	int get_index(int x, int y);
private:
	std::vector<QImage*> ImageBuffer;
	std::vector<std::vector<double>> zBuffer;
	QSize m_ImageSize;
	glm::vec3 m_lightPosition;
	glm::mat4x4* m_viewport;
	std::vector<Triangle*> m_triangles;
	std::mutex mtx;
	const QImage* m_texture = nullptr;
	MVPTransformer* m_transformer=nullptr;
	std::vector<float> depth_buf;
};
