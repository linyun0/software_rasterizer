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
#include <array>
class Triangle;
class MVPTransformer;
class TextureImage;


class RasterizationDevice
{

public:
	RasterizationDevice(const QSize& size);
	~RasterizationDevice();
	QImage* GetImage();
	void SetViewPortSize(const QSize& size);
	void StartRending();
	void FinishRender();
	void SetImageSize(const QSize& size);

	void SetTriangles(const std::vector<Triangle*>& triangles);
	void SetMVPTransformer(MVPTransformer* transformer);
	void Draw();
	void SetTextureImage(TextureImage* textureImage);
private:
	void rasterize_triangle(const Triangle& t, const std::array<Eigen::Vector3f, 3>&  view_pos);
	void set_pixel(const Eigen::Vector2i& point, const Eigen::Vector3f& color);
	int get_index(int x, int y);
private:
	std::vector<QImage*> ImageBuffer;
	QSize m_ImageSize;
	std::vector<Triangle*> m_triangles;
	MVPTransformer* m_transformer=nullptr;
	std::vector<float> depth_buf;
	TextureImage* m_textureImage = nullptr;
};
