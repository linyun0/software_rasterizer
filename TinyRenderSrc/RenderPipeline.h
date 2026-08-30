#pragma once
#include <memory>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <vector>
#include <QImage>
#include <QPainter>
#include "Triangle.h"
class ModelTransform;
class ViewTransform;
class ProjectionTransform;
class ViewportTransform;
class TextureImage;
class ThreadPool;

class RenderPipeline {

public:
	struct TriWithAABB
	{
		Triangle tri;
		std::array<Eigen::Vector3f, 3> view_pos;
		float ymin, ymax;
	};
	RenderPipeline();
	void SetModelTransform(std::shared_ptr<ModelTransform> in_modeltransformer);
	void SetViewTransform(std::shared_ptr<ViewTransform> in_camera);
	void SetProjectionTransform(std::shared_ptr<ProjectionTransform> in_projection);
	void SetViewportTransform(std::shared_ptr<ViewportTransform> in_viewport);

	void SetTextureImage(std::shared_ptr<TextureImage> in_textureImage);

	void Render(const std::vector<std::shared_ptr<Triangle>>& triangle_array,std::shared_ptr<QImage> image);
private:
	Eigen::Matrix4f GetMvpMatrix();
	Eigen::Matrix4f GetMvMatrix();
	std::vector<TriWithAABB> AfterMvpTransform(const std::vector<std::shared_ptr<Triangle>>& triangle_array);
	
private:
	
	void set_pixel(const Eigen::Vector2i& point, const Eigen::Vector3f& color);
	int get_index(int x, int y);
	std::vector<double> getScanIntersectX(const Triangle& tri, double y_scan);
	void drawFpsOnImage(double fps);
private:
	std::shared_ptr<ModelTransform> m_modeltransformer;
	std::shared_ptr<ViewTransform> m_camera;
	std::shared_ptr<ProjectionTransform> m_projection;
	std::shared_ptr<ViewportTransform> m_viewport;

	std::unique_ptr<ThreadPool> m_threadpool;
	
	std::shared_ptr<TextureImage> m_textureImage;
	std::shared_ptr<QImage> m_image;
};