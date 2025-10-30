#pragma once
#include <QWidget>
#include <QImage>
#include <QTimer>
class Camera;
class Model;
class Vertex;
class RasterizationDevice;
class Triangle;
class MVPTransformer;
class TextureImage;
class DrawAreaWidget :public QWidget
{
public:
	DrawAreaWidget(QWidget* parent);
	~DrawAreaWidget();

	void SetModeArc(const float& angle, const float& scale);
private:
	void BuildConnection();
	void BreakConnection();

private:
	void resizeEvent(QResizeEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
	void Render();
	void showEvent(QShowEvent* event) override;
private:
	Camera* m_camera=nullptr;
	QImage* m_showImage = nullptr;
	Model* m_model=nullptr;
	RasterizationDevice* m_device=nullptr;
	QTimer m_timer;
	std::vector<Triangle*> TriangleList;
	MVPTransformer* mvptransformer=nullptr;
	TextureImage* m_textureImage = nullptr;
};