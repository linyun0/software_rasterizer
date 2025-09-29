#pragma once
#include <QWidget>
#include <QImage>
#include <QTimer>
class Camera;
class Model;
class Vertex;
class RasterizationDevice;
class DrawAreaWidget :public QWidget
{
public:
	DrawAreaWidget(QWidget* parent);
	~DrawAreaWidget();

	void OpenModelFile(const QString& filePath="test");
private:
	void resizeEvent(QResizeEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
	void Render();
private:
	Camera* m_camera=nullptr;
	QImage* m_showImage = nullptr;
	Model* m_model=nullptr;
	RasterizationDevice* m_device=nullptr;
    std::vector<Vertex> mvp_perspective_vertex;
	QTimer m_timer;
};