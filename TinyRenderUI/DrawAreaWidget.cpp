#include "DrawAreaWidget.h"
#include "../TinyRenderSrc/Camera.h"
#include "../TinyRenderSrc/DataLoader.h"
#include "../TinyRenderSrc/Triangle.h"
#include "../TinyRenderSrc/MVPTransformer.h"
#include "../TinyRenderSrc/RasterizationDevice.h"
#include <qpainter.h>
DrawAreaWidget::DrawAreaWidget(QWidget* parent):QWidget(parent){

	m_camera = new Camera;
	glm::vec3 eye{ 0,0,8 };
	glm::vec3  center{ 0,0,-2 }; // camera direction
	glm::vec3  up{ 0,-1,0 }; // camera direction
	float eye_fov = 10.0; float aspect_ratio = 1.0; float zNear = 0.1; float zFar = 50.0;

	m_camera->SetCamera(eye, center, up);
	m_camera->SetViewFrustum(eye_fov,aspect_ratio,zNear,zFar); 


	m_model = new Model();
	OpenModelFile();
	
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
	m_device->Draw();
//	m_device->RenderPointsImage(mvp_perspective_vertex,QColor(255,0,0));
//	m_device->RenderWireFrameImage(mvp_perspective_vertex,QColor(255,0,0));
	
//	m_device->RenderImage(mvp_perspective_vertex); 

	//connect(&m_timer, &QTimer::timeout, this, &DrawAreaWidget::Render);
	//m_timer.start(2);

}

void DrawAreaWidget::Render() {
	//m_device->RenderImage(mvp_perspective_vertex);
	m_device->Draw();
}

void DrawAreaWidget::showEvent(QShowEvent* event)
{
	//QSize size = this->size();
	//if (m_device)
	//{
	//	m_device->SetImageSize(size);
	//	m_device->Draw();
	//}
	QWidget::showEvent(event);
}

DrawAreaWidget::~DrawAreaWidget()
{
	if (m_camera) delete m_camera;
	if (m_model) delete m_model;
}
glm::mat4x4  ry180 = { {-1, 0, 0, 0},
					{0, -1, 0, 0 },
					{ 0, 0,1, 0},
					{0, 0, 0, 1
} };

void print(glm::mat4x4 matrix)
{
	for (int i = 0; i < 4; ++i) {
		std::cout << matrix[i].x << " " << matrix[i].y << " " << matrix[i].z << " " << matrix[i].w << std::endl;
	}
	std::cout<<std::endl;
}
void DrawAreaWidget::OpenModelFile(const QString& filePath)
{
	if (filePath == "test") {
		auto lookAtMatrix = m_camera->GetLookAtMatrix();
		auto perspectiveMatrix = m_camera->GetPerspectiveMatrix();
		int meshSize = m_model->meshes.size();
		auto mvp = lookAtMatrix * perspectiveMatrix;
		auto mv = lookAtMatrix;
		for (int i = 0; i < meshSize; ++i) {
			std::vector<Vertex> vertices = m_model->meshes[i].m_vertices;
			int vertice_size = vertices.size();
			for (int j = 0; j < vertice_size; ++j)
			{
				Vertex temp;
				//glm::vec4 pos{ vertices[j].Position.x,vertices[j].Posi//tion.y,vertices[j].Position.z,1 };
				glm::vec4 pos(vertices[j].Position, 1.0f);
				//auto testpoint = lookAtMatrix * pos;
				auto mvpoint = pos *mv;
				//glm::vec4 mvpPos = perspectiveMatrix * lookAtMatrix * pos;
				glm::mat4x4 unit = {
					{1,0,0,0},
					{0,1,0,0},
					{0,0,1,0},
					{0,0,0,1},
				};
				//print(perspectiveMatrix);
				//print(lookAtMatrix);
				//print(mvp);
				glm::vec4 mvpPos = pos*mvp ;
				double w = mvpPos.w;
				mvpPos = mvpPos / (float)w;
				temp.mvPosition = mvpoint;
				temp.Position = vertices[j].Position;
				//temp.mvpPosition=glm::vec3{mvpPos.x,mvpPos.y,mvpPos.z};
				temp.mvpPosition=glm::vec3(mvpPos);
				temp.mvpPosition.x = temp.mvpPosition.x;
				temp.mvpPosition.y = temp.mvpPosition.y;
				temp.mvpPosition.z = temp.mvpPosition.z;
				glm::vec4 tempnormal{ vertices[j].Normal,1.0f };
				tempnormal = tempnormal * lookAtMatrix;
				temp.mvNormal = tempnormal;
				temp.Normal = vertices[j].Normal;
				temp.TexCoords = vertices[j].TexCoords;
				
				mvp_perspective_vertex.push_back(temp);
			}
		}




	}
	else {

	}
}

void DrawAreaWidget::SetTtriangle(std::vector<Triangle*> triangleList)
{
	TriangleList = triangleList;
}
void DrawAreaWidget::SetMVPTransformer(MVPTransformer* transformer)
{
	mvptransformer = transformer;
}
void DrawAreaWidget::Draw()
{
	


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

