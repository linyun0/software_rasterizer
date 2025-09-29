#include "DrawAreaWidget.h"
#include "../TinyRenderSrc/Camera.h"
#include "../TinyRenderSrc/DataLoader.h"
#include "../TinyRenderSrc/RasterizationDevice.h"
#include <qpainter.h>
DrawAreaWidget::DrawAreaWidget(QWidget* parent):QWidget(parent){

	m_camera = new Camera;
	vec3 eye{ 0,0,10 };
	vec3  center{ 0,0,0 }; // camera direction
	vec3  up{ 0,-1,0 }; // camera direction
	float eye_fov = 10.0; float aspect_ratio = 1.0; float zNear = 0.1; float zFar = 45.0;

	m_camera->SetCamera(eye, center, up);
	m_camera->SetViewFrustum(eye_fov,aspect_ratio,zNear,zFar); 
	
	m_model = new Model();
	OpenModelFile();

	QSize size = this->size();
	m_device = new RasterizationDevice(QSize(1000,1000));
	m_showImage = m_device->GetImage();
//	m_device->RenderPointsImage(mvp_perspective_vertex,QColor(255,0,0));
	m_device->RenderWireFrameImage(mvp_perspective_vertex,QColor(255,0,0));
	
//	m_device->RenderImage(mvp_perspective_vertex); 

//	connect(&m_timer, &QTimer::timeout, this, &DrawAreaWidget::Render);
//	m_timer.start(2);

}

void DrawAreaWidget::Render() {
	m_device->RenderImage(mvp_perspective_vertex);
}

DrawAreaWidget::~DrawAreaWidget()
{
	if (m_camera) delete m_camera;
	if (m_model) delete m_model;
}

void DrawAreaWidget::OpenModelFile(const QString& filePath)
{
	if (filePath == "test") {
		auto lookAtMatrix = m_camera->GetLookAtMatrix();
		auto perspectiveMatrix = m_camera->GetPerspectiveMatrix();
		int meshSize = m_model->meshes.size();
		for (int i = 0; i < meshSize; ++i) {
			auto vertices = m_model->meshes[i].m_vertices;
			int vertice_size = vertices.size();
			for (int j = 0; j < vertice_size; ++j)
			{
				Vertex temp;
				mat<4, 1> pos{ vertices[j].Position.x,vertices[j].Position.y,vertices[j].Position.z,1 };
				auto testpoint = lookAtMatrix * pos;
				auto mvpPos = perspectiveMatrix * lookAtMatrix * pos;
				double z = mvpPos[2][0];
				mvpPos = mvpPos / z;
				temp.Position = vertices[j].Position;
				temp.mvpPosition=vec3{mvpPos[0][0],mvpPos[1][0],mvpPos[2][0]};
				temp.Normal = vertices[j].Normal;
				temp.TexCoords = vertices[j].TexCoords;
				
				mvp_perspective_vertex.push_back(temp);
			}
		}




	}
	else {

	}
}

void DrawAreaWidget::resizeEvent(QResizeEvent* event)
{
	m_device->SetViewPortSize(this->size());
	QWidget::resizeEvent(event);

}

void DrawAreaWidget::paintEvent(QPaintEvent* event)
{

	QPainter painter(this);
	painter.drawImage(0, 0, *m_showImage);
	QWidget::paintEvent(event);

}

