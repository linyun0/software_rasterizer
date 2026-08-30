#include "DrawAreaWidget.h"
#include "../TinyRenderSrc/Camera.h"
#include "../TinyRenderSrc/DataLoader.h"
#include "../TinyRenderSrc/Triangle.h"
#include "../TinyRenderSrc/MVPTransformer.h"
#include "../TinyRenderSrc/RasterizationDevice.h"
#include "../TinyRenderSrc/Texture.h"
#include "../TinyRenderSrc/modeltransform.h"
#include "ViewTransform.h"
#include "ViewportTransform.h"
#include "modeltransform.h"
#include "ProjectionTransform.h"
#include "RenderPipeline.h"
#include <QMouseEvent>
#include <qpainter.h>
#include <QApplication>
DrawAreaWidget::DrawAreaWidget(QWidget* parent):QWidget(parent){


	m_model = new Model();
	
	mvptransformer = new MVPTransformer();
	for (auto mesh : m_model->meshes) {
		for (int i = 0; i < mesh.m_vertices.size(); i+=3) {
			//Triangle* t = new Triangle();
			std::shared_ptr<Triangle> t = std::make_shared<Triangle>();
			for (int j = 0; j < 3; j++) {
				
				t->setVertex(j, Vector4f(mesh.m_vertices[i + j].Position.x, mesh.m_vertices[i + j].Position.y, mesh.m_vertices[i + j].Position.z, 1.0));
				t->setNormal(j, Vector3f(mesh.m_vertices[i + j].Normal.x, mesh.m_vertices[i + j].Normal.y, mesh.m_vertices[i + j].Normal.z));
				t->setTexCoord(j, Vector2f(mesh.m_vertices[i + j].TexCoords.x, mesh.m_vertices[i + j].TexCoords.y));
			}
			TriangleList.emplace_back(t);
		}
	}

	//Eigen::Vector3f m_eye_pos = { 0,0,10 }, m_eye_lookat, m_eye_up;
	//float m_angle = 140, m_scalecoef = 2;
	//float m_eye_fov = 45, m_aspect_ratio = 1, m_zNear = 0.1, m_zFar = 50;
	
	m_modeltransformer = std::make_shared<ModelTransform>();
	m_camera = std::make_shared<ViewTransform>();
	m_camera->setLookAt({ 0,0,5 }, { 0,0,0 }, { 0,1,0 });
	m_projection = std::make_shared<ProjectionTransform>(45.0f,1,0.1f,100.0f);
	m_viewport = std::make_shared<ViewportTransform>(700,700,0.1,100.0f);
	m_renderpipline = std::make_shared< RenderPipeline>();
	m_renderpipline->SetModelTransform(m_modeltransformer);
	m_renderpipline->SetViewTransform(m_camera);
	m_renderpipline->SetViewportTransform(m_viewport);
	m_renderpipline->SetProjectionTransform(m_projection);
	m_textureImage = std::make_shared<TextureImage>(m_model->directory + "/spot_texture.png");
	m_renderpipline->SetTextureImage(m_textureImage);
	
	std::shared_ptr<QImage> temp = std::make_shared<QImage>(QSize(700,700), QImage::Format_ARGB32);
	
	m_renderpipline->Render(TriangleList, temp);

	//m_camera->rotateAroundTargetByMatrix(Eigen::Vector3f::UnitY(), 90);
	//m_camera->rotateAroundTargetByMatrix(Eigen::Vector3f::UnitX(), 90);
	//m_camera->setLookAt({ 0,0,5 }, { 0,0,0 }, { 0,-1,0 });
	m_camera->rotateAroundY(90);
	m_renderpipline->Render(TriangleList, temp);


	QSize size = this->size();
	m_device = new RasterizationDevice(QSize(700,700));
	m_showImage = m_device->GetImage();
	m_device->SetTriangles(TriangleList);
	m_device->SetMVPTransformer(mvptransformer);


	


//	m_device->SetTextureImage(m_textureImage);
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

void DrawAreaWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_mouseDown = true;
		m_lastMousePos = event->pos();
	}
	QWidget::mousePressEvent(event);
}

void DrawAreaWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (!m_mouseDown)
		return;

	QPoint currentPos = event->pos();
	QPoint delta = currentPos - m_lastMousePos;

	// 判断是否按下Ctrl键
	bool ctrlPressed = (event->modifiers() & Qt::ControlModifier);

	if (ctrlPressed)
	{
		//==== Ctrl + 左键拖动：平移 Pan ====
		// delta.x：鼠标向右 → 相机向右；delta.y：鼠标向下 → 相机向上
		float right = -delta.x() * m_panSensitivity;
		float up = delta.y() * m_panSensitivity;
		m_camera->panCameraByMatrix(right, up);
	}
	else
	{
		//==== 普通左键拖动：绕Y、X旋转 ====
		// 鼠标左右移动 → rotateAroundY(Yaw)；鼠标上下移动 → rotateAroundX(Pitch)
		float deltaYaw = delta.x() * m_rotateSensitivity;
		float deltaPitch = delta.y() * m_rotateSensitivity;

		m_camera->rotateAroundY(deltaYaw);
		m_camera->rotateAroundX(deltaPitch);
	}

	m_lastMousePos = currentPos;

	// 触发重新渲染，你调用自己的渲染入口
	// 如果你是 m_renderpipline 做渲染，这里需要触发一帧Render；
	// 如果你是旧的m_device设备，调用 Draw();
//#if 1
	std::shared_ptr<QImage> tempImage = std::make_shared<QImage>(QSize(700, 700), QImage::Format_ARGB32);
	m_renderpipline->Render(TriangleList, tempImage);
//#endif
//	m_device->Draw();
//	this->update();

	QWidget::mouseMoveEvent(event);
}

void DrawAreaWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_mouseDown = false;
	}
	QWidget::mouseReleaseEvent(event);
}