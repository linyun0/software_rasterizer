#include "Camera.h"
#include <cmath>
#define _USE_MATH_DEFINES
void Camera::SetViewFrustum(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
	m_zfov = eye_fov;
	m_aspect = aspect_ratio;
	m_zNear = zNear;
	m_zFar = zFar;

	mat<4, 4> M_trans;
	mat<4, 4> M_persp;
	mat<4, 4> M_ortho;
	M_persp = { {
		{zNear, 0, 0, 0},
		{0, zNear, 0, 0},
		{0, 0, zNear + zFar, -zFar * zNear},
		{0, 0, 1, 0} }
	};

	float alpha = 0.5 * eye_fov * std::acos(-1.0) / 180.0f;
	float yTop = -zNear * std::tan(alpha); //
	float yBottom = -yTop;
	float xRight = yTop * aspect_ratio;
	float xLeft = -xRight;

	M_trans = { {{
		1, 0, 0, -(xLeft + xRight) / 2},
		{0, 1, 0, -(yTop + yBottom) / 2},
		{0, 0, 1, -(zNear + zFar) / 2},
		{0, 0, 0, 1}} };
	M_ortho = { {
		{2 / (xRight - xLeft), 0, 0, 0},
		{0, 2 / (yTop - yBottom), 0, 0},
		{0, 0, 2 / (zNear - zFar), 0 },
		{0, 0, 0, 1} }
	};

	M_ortho = M_ortho * M_trans;
	m_perspectiveMatrix = M_ortho * M_persp;

	//	GenerateProjectionMatrix();
}

void Camera::SetCamera(const vec3& position, const vec3& target,const vec3& up)
{
	m_position = position;
	m_target = target;
	GenerateLookAtMatrix();
}

void Camera::RotateAroundTarget(vec2 motion)
{

}
void Camera::MoveTarget(vec2 motion)
{

}
void  Camera::CloseToTarget(int ratio)
{

}
void  Camera::SetModel(vec3 modelCentre, float yRange)
{
}
mat<4, 4>  Camera::GetLookAtMatrix()  //Camera matrix
{
	return m_lookAtMatrix;
}
mat<4, 4> Camera::GetPerspectiveMatrix()
{
	return  m_perspectiveMatrix;
}
mat<4, 4> Camera::GetOrthographicMatrix()
{
	return  m_orthographicMatrix;
}


void Camera::GenerateLookAtMatrix()
{
	vec<3> CameraZ = m_target - m_position;  //×óÊÖ×ø±êÏµ
	//CameraZ = vec<3>{ 0,0,0 }-CameraZ;
	vec<3> CameraZ_normal = normalized(CameraZ);

	vec<3> CameraX = cross(m_updirection, CameraZ_normal);
	//CameraX = vec<3>{ 0,0,0 }- normalized(CameraX);
	vec<3> CameraX_normal = normalized(CameraX);

	//	vec<3> CameraY = cross(CameraZ_normal, CameraX_normal);
	m_updirection = vec<3>{ 0,0,0 } - m_updirection;
	vec<3> CameraY_normal = normalized(m_updirection);

	mat<4, 4> Camera{ {
		{CameraX_normal.x,CameraY_normal.x,CameraZ_normal.x,0},
		{CameraX_normal.y,CameraY_normal.y,CameraZ_normal.y,0},
		{CameraX_normal.z,CameraY_normal.z,CameraZ_normal.z,0},
		{0		  ,0		,0		  ,1}
	} };

	mat<4, 4> MoveToPosition{ {
		{1,0,0,-m_position.x},
		{0,1,0,-m_position.y},
		{0,0,1,-m_position.z},
		{0,0,0,1}
	} };

	mat<4, 4> Camera_T = Camera.transpose();

	mat<4, 4> ry180 = { { {-1, 0, 0, 0},
						{0, 1, 0, 0 },
						{ 0, 0,-1, 0},
						{0, 0, 0, 1}
	} };
	mat<4, 4> ry90 = { { {0, 0, 1, 0},
						{0, 1, 0, 0 },
						{ -1, 0,0, 0},
						{0, 0, 0, 1}
	} };
	mat<4, 4> rx180 = { { {1, 0, 0, 0},
						{0, -1, 0, 0 },
						{ 0, 0,-1, 0},
						{0, 0, 0, 1}
	} };

	m_lookAtMatrix = Camera_T * MoveToPosition;

}

void Camera::GenerateProjectionMatrix()
{
	mat<4, 4> M_trans;
	mat<4, 4> M_persp;
	mat<4, 4> M_ortho;
	M_persp = { {
		{m_zNear, 0, 0, 0},
		{0, m_zNear, 0, 0},
		{0, 0, m_zNear + m_zFar, -m_zFar * m_zNear},
		{0, 0, 1, 0} }
	};

	float alpha = 0.5 * m_zfov * std::acos(-1.0) / 180.0f;
	float yTop = -m_zNear * std::tan(alpha); //
	float yBottom = -yTop;
	float xRight = yTop * m_aspect;
	float xLeft = -xRight;

	M_trans = { {{
		1, 0, 0, -(xLeft + xRight) / 2},
		{0, 1, 0, -(yTop + yBottom) / 2},
		{0, 0, 1, -(m_zNear + m_zFar) / 2},
		{0, 0, 0, 1}} };
	M_ortho = { {
		{2 / (xRight - xLeft), 0, 0, 0},
		{0, 2 / (yTop - yBottom), 0, 0},
		{0, 0, 2 / (m_zNear - m_zFar), 0 },
		{0, 0, 0, 1} }
	};

	m_orthographicMatrix = M_ortho * M_trans;
	m_perspectiveMatrix = M_ortho * M_persp;

}
