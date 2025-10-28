#include "Camera.h"
#include <cmath>
#include <string>
#define _USE_MATH_DEFINES
void prints(glm::mat4x4 matrix)
{
	for (int i = 0; i < 4; ++i) {
		std::cout << matrix[i].x << " " << matrix[i].y << " " << matrix[i].z << " " << matrix[i].w << std::endl;
	}
	std::cout << std::endl;
}

void Camera::SetViewFrustum(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
	m_zfov = eye_fov;
	m_aspect = aspect_ratio;
	m_zNear = zNear;
	m_zFar = zFar;

	glm::mat4x4 Indentity = {
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1},
	};

	glm::mat4x4  M_trans;
	glm::mat4x4  M_persp;
	glm::mat4x4  M_ortho;
	M_persp = { 
		{zNear, 0, 0, 0},
		{0, zNear, 0, 0},
		{0, 0, zNear + zFar, -zFar * zNear},
		{0, 0, 1, 0} 
	};

	float alpha = 0.5 * eye_fov * std::acos(-1.0) / 180.0f;
	float yTop = -zNear * std::tan(alpha); //
	float yBottom = -yTop;
	float xRight = yTop * aspect_ratio;
	float xLeft = -xRight;

	M_trans = { {
		1, 0, 0, -(xLeft + xRight) / 2},
		{0, 1, 0, -(yTop + yBottom) / 2},
		{0, 0, 1, -(zNear + zFar) / 2},
		{0, 0, 0, 1}} ;
	M_ortho = { 
		{2 / (xRight - xLeft), 0, 0, 0},
		{0, 2 / (yTop - yBottom), 0, 0},
		{0, 0, 2 / (zNear - zFar), 0 },
		{0, 0, 0, 1} 
	};
	

	
	m_perspectiveMatrix = Indentity* M_persp*M_trans*M_ortho;
//	m_perspectiveMatrix = Indentity*M_ortho*M_trans* M_persp;

	
}

void Camera::SetCamera(const glm::vec3& position, const glm::vec3& target,const glm::vec3& up)
{
	m_position = position;
	m_target = target;
	GenerateLookAtMatrix();
}

void Camera::RotateAroundTarget(glm::vec2 motion)
{

}
void Camera::MoveTarget(glm::vec2 motion)
{

}
void  Camera::CloseToTarget(int ratio)
{

}
void  Camera::SetModel(glm::vec3 modelCentre, float yRange)
{
}
glm::mat4x4   Camera::GetLookAtMatrix()  //Camera matrix
{
	return m_lookAtMatrix;
}
glm::mat4x4  Camera::GetPerspectiveMatrix()
{
	return  m_perspectiveMatrix;
}
glm::mat4x4  Camera::GetOrthographicMatrix()
{
	return  m_orthographicMatrix;
}


void Camera::GenerateLookAtMatrix()
{
	glm::mat4x4  ry180 = { {-1, 0, 0, 0},
					{0, 1, 0, 0 },
					{ 0, 0,-1, 0},
					{0, 0, 0, 1
} };
	glm::mat4x4  rx180 = { {1, 0, 0, 0},
						{0, -1, 0, 0 },
						{ 0, 0,-1, 0},
						{0, 0, 0, 1}
	};
	glm::mat4x4  rz180 = { {-1, 0, 0, 0},
						{0, -1, 0, 0 },
						{ 0, 0,1, 0},
						{0, 0, 0, 1}
	};


	glm::vec3 CameraZ = m_target - m_position;  //×óÊÖ×ø±êÏµ
	glm::vec3 newCameraZ = glm::vec4(CameraZ, 1.0f) ;
	glm::vec3 CameraZ_normal =glm::normalize(newCameraZ);
	
	glm::vec3 CameraX = glm::cross(m_updirection, CameraZ_normal);
	glm::vec3 CameraX_normal =glm::normalize(CameraX);

	m_updirection = glm::vec3{0,0,0} - m_updirection;
	glm::vec3 CameraY_normal = glm::normalize(m_updirection);

	glm::mat4x4  Camera{
		{CameraX_normal.x,CameraY_normal.x,CameraZ_normal.x,0},
		{CameraX_normal.y,CameraY_normal.y,CameraZ_normal.y,0},
		{CameraX_normal.z,CameraY_normal.z,CameraZ_normal.z,0},
		{0		  ,0		,0		  ,1}
	} ;

	//Camera = rx180 * Camera;

	glm::mat4x4  MoveToPosition{ 
		{1,0,0,-m_position.x},
		{0,1,0,-m_position.y},
		{0,0,1,-m_position.z},
		{0,0,0,1}
	} ;
	
	glm::mat4x4  Camera_T = glm::transpose(Camera);


	//m_lookAtMatrix = Camera_T * MoveToPosition;
	m_lookAtMatrix = MoveToPosition*Camera_T;

	auto	temp= glm::mat4x4{ 
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,-10},
		{0,0,0,1}
	};
	
//	m_lookAtMatrix = temp*rz180;
	//m_lookAtMatrix = temp;
	//m_lookAtMatrix = m_lookAtMatrix;
}

void Camera::GenerateProjectionMatrix()
{
	glm::mat4x4  M_trans;
	glm::mat4x4  M_persp;
	glm::mat4x4  M_ortho;
	M_persp = {
		{m_zNear, 0, 0, 0},
		{0, m_zNear, 0, 0},
		{0, 0, m_zNear + m_zFar, -m_zFar * m_zNear},
		{0, 0, 1, 0} }
	;

	float alpha = 0.5 * m_zfov * std::acos(-1.0) / 180.0f;
	float yTop = -m_zNear * std::tan(alpha); //
	float yBottom = -yTop;
	float xRight = yTop * m_aspect;
	float xLeft = -xRight;

	M_trans = {{
		1, 0, 0, -(xLeft + xRight) / 2},
		{0, 1, 0, -(yTop + yBottom) / 2},
		{0, 0, 1, -(m_zNear + m_zFar) / 2},
		{0, 0, 0, 1}} ;
	M_ortho = { 
		{2 / (xRight - xLeft), 0, 0, 0},
		{0, 2 / (yTop - yBottom), 0, 0},
		{0, 0, 2 / (m_zNear - m_zFar), 0 },
		{0, 0, 0, 1} };

	m_orthographicMatrix = M_ortho * M_trans;
	m_perspectiveMatrix = M_ortho * M_persp;

}
