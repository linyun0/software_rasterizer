#pragma once
#include <QWidget>
#include <QImage>
#include <QTimer>
#include <memory>
#include <QColor>
class Camera;
class Model;
class Vertex;
class RasterizationDevice;
class Triangle;
class MVPTransformer;
class TextureImage;

class ModelTransform;
class ViewTransform;
class ProjectionTransform;
class ViewportTransform;
class RenderPipeline;

class DrawAreaWidget :public QWidget
{
public:
	DrawAreaWidget(QWidget* parent);
	~DrawAreaWidget();

	void LightColorChanged(QColor color);
private:
	void BuildConnection();
	void BreakConnection();

private:
	void resizeEvent(QResizeEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
	void Render();
	void showEvent(QShowEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
private:
	
	std::shared_ptr<QImage> m_showImage;
	std::shared_ptr<Model> m_model;


	std::vector<std::shared_ptr<Triangle>> TriangleList;
	std::shared_ptr<TextureImage> m_textureImage;
	std::shared_ptr<ModelTransform> m_modeltransformer;
	std::shared_ptr<ViewTransform> m_camera;
	std::shared_ptr<ProjectionTransform> m_projection;
	std::shared_ptr<ViewportTransform> m_viewport;
	std::shared_ptr<RenderPipeline> m_renderpipline;
private:
	//鼠标交互状态
	bool m_mouseDown = false;
	QPoint m_lastMousePos;
	float m_rotateSensitivity = 0.25f;  //旋转灵敏度，调小转动慢
	float m_panSensitivity = 0.005f;    //平移灵敏度
};