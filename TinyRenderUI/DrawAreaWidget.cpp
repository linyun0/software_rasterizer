#include "DrawAreaWidget.h"
#include "../TinyRenderSrc/Camera.h"
#include "../TinyRenderSrc/DataLoader.h"
#include "../TinyRenderSrc/Triangle.h"
#include "../TinyRenderSrc/MVPTransformer.h"
#include "../TinyRenderSrc/RasterizationDevice.h"
#include "../TinyRenderSrc/Texture.h"
#include <qpainter.h>
#include <QApplication>
DrawAreaWidget::DrawAreaWidget(QWidget* parent):QWidget(parent){


	m_model = new Model();
	
	mvptransformer = new MVPTransformer();
	for (auto mesh : m_model->meshes) {
		for (int i = 0; i < mesh.m_vertices.size(); i+=3) {
			Triangle* t = new Triangle();
			for (int j = 0; j < 3; j++) {
				
				t->setVertex(j, Vector4f(mesh.m_vertices[i + j].Position.x, mesh.m_vertices[i + j].Position.y, mesh.m_vertices[i + j].Position.z, 1.0));
				t->setNormal(j, Vector3f(mesh.m_vertices[i + j].Normal.x, mesh.m_vertices[i + j].Normal.y, mesh.m_vertices[i + j].Normal.z));
				t->setTexCoord(j, Vector2f(mesh.m_vertices[i + j].TexCoords.x, mesh.m_vertices[i + j].TexCoords.y));
			}
			TriangleList.push_back(t);
		}
	}
	
		

	QSize size = this->size();
	m_device = new RasterizationDevice(QSize(700,700));
	m_showImage = m_device->GetImage();
	m_device->SetTriangles(TriangleList);
	m_device->SetMVPTransformer(mvptransformer);


	m_textureImage = new TextureImage("D:/spot_texture.png");
	m_device->SetTextureImage(m_textureImage);
	//connect(&m_timer, &QTimer::timeout, this,[=](){
	//	m_testangle += 30;
	//	if (m_testangle > 360) {
	//		m_testangle = 0;
	//	}
	//	mvptransformer->SetModelArc(m_testangle, 3);
	//	m_device->Draw();
	//	});
	//m_timer.start(10);

}

void DrawAreaWidget::Render() {
	m_device->Draw();
}

void DrawAreaWidget::showEvent(QShowEvent* event)
{
	QSize size = this->size();
	m_device->SetImageSize(size);
	m_device->Draw();
	QWidget::showEvent(event);
}

DrawAreaWidget::~DrawAreaWidget()
{
	if (m_camera) delete m_camera;
	if (m_model) delete m_model;
}


	


void DrawAreaWidget::SetModeArc(const float& angle, const float& scale)
{
	mvptransformer->SetModelArc(angle, scale);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	m_device->Draw();
	this->update();
	QApplication::restoreOverrideCursor();
}
void DrawAreaWidget::BuildConnection()
{
}
void DrawAreaWidget::BreakConnection()
{
}
void DrawAreaWidget::resizeEvent(QResizeEvent* event)
{
	QSize size = this->size();
	bool isShow = this->isVisible();
	if (isShow) {
		m_device->SetImageSize(size);
		m_device->Draw();
	}
	QWidget::resizeEvent(event);

}

void DrawAreaWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.drawImage(0, 0, *m_showImage);
	QWidget::paintEvent(event);

}

