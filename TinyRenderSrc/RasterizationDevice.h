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
class RasterizationDevice
{

public:
	RasterizationDevice(const QSize& size);
	~RasterizationDevice();
	QImage* GetImage();
	void SetViewPortSize(const QSize& size);
	void StartRending();
	std::vector<int> GetBoundingBox(const std::vector<vec3>& triangle);
	std::vector<std::vector<int>> ScalLine(const std::vector<vec3>& triangle);
	bool isInTriangle(const std::vector<int>& point, const std::vector<vec3>& triangel);
	void SetLightDirection(const std::vector<double>& lightDirection);
	bool crossProduct(const double& x1, const  double& y1, const double& x2, const double& y2);
	std::vector<double> baryCentric(const std::vector<int>& point, const std::vector<vec3>& triangle);
	void FinishRender();
	void RenderPointsImage(const std::vector<Vertex>& arrayVertex,const QColor& color=QColor(255,0,0))
	{
		StartRending();
		int size = arrayVertex.size();
		for (int i = 0; i < size; ++i) {
			mat<4, 1> pos{ arrayVertex[i].mvpPosition.x,arrayVertex[i].mvpPosition.y ,arrayVertex[i].mvpPosition.z ,1 };
			auto drawPos = *(m_viewport)*pos;
			ImageBuffer[1]->setPixelColor(drawPos[0][0], drawPos[1][0]
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
			mat<4, 1> pos{ arrayVertex[i].mvpPosition.x,arrayVertex[i].mvpPosition.y ,arrayVertex[i].mvpPosition.z ,1 };
			auto drawPos = *(m_viewport)*pos;
			mat<4, 1> pos1{ arrayVertex[i+1].mvpPosition.x,arrayVertex[i+1].mvpPosition.y ,arrayVertex[i+1].mvpPosition.z ,1 };
			auto drawPos1 = *(m_viewport)*pos1;
			mat<4, 1> pos2{ arrayVertex[i+2].mvpPosition.x,arrayVertex[i+2].mvpPosition.y ,arrayVertex[i+2].mvpPosition.z ,1 };
			auto drawPos2 = *(m_viewport)*pos2;
			
			painter.drawLine(drawPos[0][0],drawPos[1][0], drawPos1[0][0],drawPos1[1][0]);
			painter.drawLine(drawPos1[0][0],drawPos1[1][0], drawPos2[0][0],drawPos2[1][0]);
			painter.drawLine(drawPos2[0][0],drawPos2[1][0], drawPos[0][0],drawPos[1][0]);

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
			mat<4, 1> pos{ arrayVertex[i].mvpPosition.x,arrayVertex[i].mvpPosition.y ,arrayVertex[i].mvpPosition.z ,1 };
			mat<4,1> drawPos = *(m_viewport)*pos;
			mat<4, 1> pos1{ arrayVertex[i + 1].mvpPosition.x,arrayVertex[i + 1].mvpPosition.y ,arrayVertex[i + 1].mvpPosition.z ,1 };
			mat<4,1> drawPos1 = *(m_viewport)*pos1;
			mat<4, 1> pos2{ arrayVertex[i + 2].mvpPosition.x,arrayVertex[i + 2].mvpPosition.y ,arrayVertex[i + 2].mvpPosition.z ,1 };
			mat<4,1> drawPos2 = *(m_viewport)*pos2;

			vec3 point1 = vec3{ drawPos[0][0],drawPos[1][0],drawPos[2][0] };
			vec3 point2 = vec3{ drawPos1[0][0],drawPos1[1][0],drawPos1[2][0] };
			vec3 point3 = vec3{ drawPos2[0][0],drawPos2[1][0],drawPos2[2][0] };
			std::vector<vec3> pointarray{ point1,point2,point3 };
			
			std::vector<int> BoundingBox= GetBoundingBox(pointarray);
			//#pragma omp parallel for
			vec3 oripoint1 = arrayVertex[i].Position;
			vec3 oripoint2 = arrayVertex[i+1].Position;
			vec3 oripoint3 = arrayVertex[i+2].Position;
			auto normal1 = arrayVertex[i].Normal;
			auto normal2 = arrayVertex[i+1].Normal;
			auto normal3 = arrayVertex[i+2].Normal;

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
				std::vector<double> args = baryCentrictemp(point1.x, point1.y, point2.x, point2.y, point3.x, point3.y, x, y);
				if (args[0] < 0 || args[1] < 0 || args[2] < 0) {
					continue;
				};
				double pointZ = args[0] *oripoint1.z +
					args[1] * oripoint2.z +
					args[2] * oripoint3.z;
				if (pointZ < zBuffer[x][y]) {
					continue;
				}
				zBuffer[x][y] = pointZ;
				
				auto start = std::chrono::high_resolution_clock::now();
				//light
				vec3 currentPoint = args[0] * oripoint1+ args[1] * oripoint2+ args[2] *oripoint3;
			//	double r = calculateDistance(currentPoint, vec3{ m_lightDirection[0],m_lightDirection[1],m_lightDirection[2] });

				vec3 newNormal = normal1 * args[0] + normal2 * args[1] + normal3 * args[2];
				double intensity = m_lightDirection[0] * newNormal[0] +
					m_lightDirection[1] * newNormal[1] +
					m_lightDirection[2] * newNormal[2];
				intensity = std::max(0.0, std::min(intensity, 1.0));
				//if (std::abs(intensity - 0) == 0) {
				//	continue;
				//}
			//	double newintensity = (1 / (r * r)) * intensity;
			//	intensity = newintensity * 3;
				// intensity = (1/(r*r))*intensity;

				vec2 realuv = uv1 * args[0] + uv2 * args[1] + uv3 * args[2];
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
					color = QColor(intensity * 255,
						intensity * 255,
						intensity * 255, 255);
				}


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
		ImageBuffer[1]->save("test.png");
	}
	void RenderTriangle()
	{
	}
//	void rendingTriangles(const std::vector<triangle>& triangles, const QImage* image = nullptr);
//	void rendingTriangle(const triangle& singletriangle, const bool& isframe = false);
	std::vector<vec3> ViewPortTransform(mat<4, 3>  m_triangleMatrix);


	std::vector<double> baryCentrictemp(const int& x1, const int& y1, const int& x2, const int& y2, const int& x3, const int& y3, const int& px, const int& py)
	{
		double ax = x1, ay = y1;
		double bx = x2, by = y2;
		double cx = x3, cy = y3;
		double px_d = px, py_d = py;

		double abx = bx - ax;
		double aby = by - ay;
		double acx = cx - ax;
		double acy = cy - ay;
		double apx = px_d - ax;
		double apy = py_d - ay;

		double SABC = abx * acy - aby * acx;
		double SABP = abx * apy - aby * apx;  // AB × AP (对应gamma)
		double SAPC = apx * acy - apy * acx;   // AP × AC (对应beta)

		if (std::abs(SABC) < 1e-5) {
			return { 0.0, 0.0, 0.0 };
		}

		double beta = SAPC / SABC;      // 顶点B的权重
		double gamma = SABP / SABC;     // 顶点C的权重
		double alpha = 1.0 - beta - gamma; // 顶点A的权重

		return { alpha, beta, gamma };
	}

	mat<4, 4> GetViewPort()
	{
		return *m_viewport;
	}

private:
	void SetViewPort()
	{
		double x = m_ImageSize.width() / 8.0;
		double y = m_ImageSize.height() / 8.0;
		double w = 6 * x;
		double h = 6 * y;

		//w *= 4;
		//h *= 4;

		m_viewport = new mat<4, 4>{ {{w / 2., 0, 0, x + w / 2.},
			{0, h / 2., 0, y + h / 2.},
			{0,0,1,0},
			{0,0,0,1}} };
		zBuffer.clear();
		for (int i = 1; i < m_ImageSize.width() + 1; ++i) {
			zBuffer.push_back(std::vector<double>(m_ImageSize.height() + 1, INT_MIN));
		}

	}

	void testSave();

private:
	std::vector<QImage*> ImageBuffer;
	std::vector<std::vector<double>> zBuffer;
	QSize m_ImageSize;
	std::vector<double> m_lightDirection;
	mat<4, 4>* m_viewport;

	std::mutex mtx;
	const QImage* m_texture = nullptr;
};
