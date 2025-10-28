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
class DrawAreaWidget :public QWidget
{
public:
	DrawAreaWidget(QWidget* parent);
	~DrawAreaWidget();

	void OpenModelFile(const QString& filePath="test");
	void SetTtriangle(std::vector<Triangle*> triangleList);
	void SetMVPTransformer(MVPTransformer* transformer);
	void Draw();
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
    std::vector<Vertex> mvp_perspective_vertex;
	QTimer m_timer;
	std::vector<Triangle*> TriangleList;
	MVPTransformer* mvptransformer=nullptr;
};