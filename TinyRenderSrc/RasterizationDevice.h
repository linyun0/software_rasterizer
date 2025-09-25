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
			mat<4, 1> pos{ arrayVertex[i].Position.x,arrayVertex[i].Position.y ,arrayVertex[i].Position.z ,1 };
			auto drawPos = *(m_viewport)*pos;
			ImageBuffer[1]->setPixelColor(drawPos[0][0], drawPos[1][0]
				, color);

		}
		ImageBuffer[1]->save("test.png");
		FinishRender();

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
