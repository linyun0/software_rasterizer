#include "DrawAreaWidget.h"
#include "DataLoader.h"
#include "Triangle.h"
#include "Texture.h"
#include "ViewTransform.h"
#include "ViewportTransform.h"
#include "modeltransform.h"
#include "ProjectionTransform.h"
#include "RenderPipeline.h"
#include <QMouseEvent>
#include <qpainter.h>
#include <QApplication>
#include "threadpool.h"
DrawAreaWidget::DrawAreaWidget(QWidget* parent):QWidget(parent){


	m_model = std::make_shared<Model>();
	

	for (auto mesh : m_model->meshes) {
		for (int i = 0; i < mesh.m_vertices.size(); i+=3) {
			std::shared_ptr<Triangle> t = std::make_shared<Triangle>();
			for (int j = 0; j < 3; j++) {
				
				t->setVertex(j, Vector4f(mesh.m_vertices[i + j].Position.x, mesh.m_vertices[i + j].Position.y, mesh.m_vertices[i + j].Position.z, 1.0));
				t->setNormal(j, Vector3f(mesh.m_vertices[i + j].Normal.x, mesh.m_vertices[i + j].Normal.y, mesh.m_vertices[i + j].Normal.z));
				t->setTexCoord(j, Vector2f(mesh.m_vertices[i + j].TexCoords.x, mesh.m_vertices[i + j].TexCoords.y));
			}
			TriangleList.emplace_back(t);
		}
	}

	
	m_modeltransformer = std::make_shared<ModelTransform>();
	m_camera = std::make_shared<ViewTransform>();
	m_camera->setLookAt({ 0,0,5 }, { 0,0,0 }, { 0,1,0 });

	float ration = this->width() / this->height();

	m_projection = std::make_shared<ProjectionTransform>(45.0f,ration,0.1f,100.0f);
	m_viewport = std::make_shared<ViewportTransform>(this->width(), this->height(), 0.1, 100.0f);
	m_renderpipline = std::make_shared< RenderPipeline>();
	m_renderpipline->SetModelTransform(m_modeltransformer);
	m_renderpipline->SetViewTransform(m_camera);
	m_renderpipline->SetViewportTransform(m_viewport);
	m_renderpipline->SetProjectionTransform(m_projection);
	m_textureImage = std::make_shared<TextureImage>(m_model->directory + "/spot_texture.png");
	m_renderpipline->SetTextureImage(m_textureImage);
	
}

void DrawAreaWidget::Render() {

	int w = this->width();
	int h = this->height();
	if (h <= 0 || w <= 0)
		return;

	// ? 同时更新投影 + 视口！两者必须成对更新
	float aspect = static_cast<float>(w) / static_cast<float>(h);
	m_projection->SetAspect(aspect);
	m_viewport->SetWidth(w);
	m_viewport->SetHeight(h);

	std::shared_ptr<QImage> tempImage = std::make_shared<QImage>(QSize(w, h), QImage::Format_ARGB32);
	m_renderpipline->Render(TriangleList, tempImage);
	m_showImage = tempImage;
}

void DrawAreaWidget::showEvent(QShowEvent* event)
{

	QWidget::showEvent(event);
}

DrawAreaWidget::~DrawAreaWidget()
{

}


	


void DrawAreaWidget::SetModeArc(const float& angle, const float& scale)
{
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

	QWidget::resizeEvent(event);

}

void DrawAreaWidget::paintEvent(QPaintEvent* event)
{
	Render();
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



	this->update();


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