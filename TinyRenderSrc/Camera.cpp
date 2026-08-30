#include "Camera.h"
#include <cmath>
#include <string>
#include <iostream>
#include "geometry.h"

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
	GenerateProjectionMatrix(); //调用生成投影矩阵，不要手动写矩阵相乘
}

void Camera::SetCamera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
{
	m_position = position;
	m_target = target;
	m_updirection = up;
	GenerateLookAtMatrix(); //更新view矩阵
}

/**
 * @brief RotateAroundTarget 环绕目标旋转，鼠标左键拖拽
 * @param motion 鼠标位移增量，motion.x水平旋转，motion.y俯仰旋转
 * 思路：球坐标，m_target固定不动，只修改m_position
 */
void Camera::RotateAroundTarget(glm::vec2 motion)
{
	glm::vec3 fromTarget = m_position - m_target;
	float radius = glm::length(fromTarget);
	if (radius < 1e-6f)
		return;

	//球坐标 yaw水平，pitch俯仰
	float yaw = std::atan2(fromTarget.x, fromTarget.z);
	float pitch = std::asin(fromTarget.y / radius);

	const float factor = static_cast<float>(MY_PI * 2.0f);
	yaw -= motion.x * factor;
	pitch += motion.y * factor;

	//俯仰角限位 ±89.9度，防止万向锁翻转
	const float limit = glm::radians(89.9f);
	if (pitch < -limit) pitch = -limit;
	if (pitch > limit) pitch = limit;

	//球坐标转回笛卡尔
	glm::vec3 offset;
	offset.x = radius * std::cos(pitch) * std::sin(yaw);
	offset.y = radius * std::sin(pitch);
	offset.z = radius * std::cos(pitch) * std::cos(yaw);

	m_position = m_target + offset;

	//位置修改后，重新生成LookAt矩阵
	GenerateLookAtMatrix();
}

/**
 * @brief MoveTarget 鼠标中键拖拽平移，相机+目标一起移动
 * @param motion 鼠标位移增量
 */
void Camera::MoveTarget(glm::vec2 motion)
{
	glm::vec3 fromPos = m_target - m_position;
	glm::vec3 forward = glm::normalize(fromPos);
	glm::vec3 worldUp{ 0,1,0 };

	glm::vec3 left = glm::cross(worldUp, forward);
	left = glm::normalize(left);
	glm::vec3 upDir = glm::normalize(glm::cross(forward, left));

	float distance = glm::length(fromPos);
	float alpha = glm::radians(m_zfov * 0.5f);
	float factor = distance * std::tan(alpha) * 2.0f;

	glm::vec3 deltaX = factor * m_aspect * motion.x * left;
	glm::vec3 deltaY = factor * motion.y * upDir;
	glm::vec3 delta = deltaX + deltaY;

	m_target += delta;
	m_position += delta;

	GenerateLookAtMatrix();
}

/**
 * @brief CloseToTarget 鼠标滚轮缩放
 * @param ratio 滚轮滚动步数，正数靠近，负数远离
 */
void Camera::CloseToTarget(int ratio)
{
	glm::vec3 fromTarget = m_position - m_target;
	float radius = glm::length(fromTarget);
	if (radius < 1e-6f)
		return;

	float yaw = std::atan2(fromTarget.x, fromTarget.z);
	float pitch = std::asin(fromTarget.y / radius);

	//0.95每滚轮一次缩放系数
	radius *= std::pow(0.95f, static_cast<float>(ratio));

	glm::vec3 offset;
	offset.x = radius * std::cos(pitch) * std::sin(yaw);
	offset.y = radius * std::sin(pitch);
	offset.z = radius * std::cos(pitch) * std::cos(yaw);

	m_position = m_target + offset;
	GenerateLookAtMatrix();
}

/**
 * @brief SetModel 将相机对准模型包围盒中心
 * @param modelCentre 模型中心
 * @param yRange 模型Y轴半高
 */
void Camera::SetModel(glm::vec3 modelCentre, float yRange)
{
	m_target = modelCentre;
	m_position = modelCentre;
	float alpha = glm::radians(m_zfov * 0.5f);
	m_position.z += yRange / std::tan(alpha);

	GenerateLookAtMatrix();
}

glm::mat4x4 Camera::GetLookAtMatrix()
{
	return m_lookAtMatrix;
}

glm::mat4x4 Camera::GetPerspectiveMatrix()
{
	return  m_perspectiveMatrix;
}

glm::mat4x4 Camera::GetOrthographicMatrix()
{
	return  m_orthographicMatrix;
}

// 左手系LookAt生成（原有逻辑保留）
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
	glm::vec3 CameraZ = m_target - m_position;  //左手坐标系
	glm::vec3 newCameraZ = CameraZ;
	glm::vec3 CameraZ_normal = glm::normalize(newCameraZ);

	glm::vec3 CameraX = glm::cross(m_updirection, CameraZ_normal);
	glm::vec3 CameraX_normal = glm::normalize(CameraX);
	glm::vec3 CameraY_normal = glm::normalize(glm::cross(CameraZ_normal, CameraX_normal));

	glm::mat4x4  Camera{
		{CameraX_normal.x,CameraY_normal.x,CameraZ_normal.x,0},
		{CameraX_normal.y,CameraY_normal.y,CameraZ_normal.y,0},
		{CameraX_normal.z,CameraY_normal.z,CameraZ_normal.z,0},
		{0		  ,0		,0		  ,1}
	};

	glm::mat4x4  MoveToPosition{
		{1,0,0,-m_position.x},
		{0,1,0,-m_position.y},
		{0,0,1,-m_position.z},
		{0,0,0,1}
	};

	glm::mat4x4  Camera_T = glm::transpose(Camera);
	m_lookAtMatrix = MoveToPosition * Camera_T;
}

// 生成投影矩阵，修复原代码矩阵相乘顺序错误
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
	float alpha = 0.5f * m_zfov * static_cast<float>(MY_PI) / 180.0f;
	float yTop = -m_zNear * std::tan(alpha);
	float yBottom = -yTop;
	float xRight = yTop * m_aspect;
	float xLeft = -xRight;

	M_trans = { {
		1, 0, 0, -(xLeft + xRight) / 2.f},
		{0, 1, 0, -(yTop + yBottom) / 2.f},
		{0, 0, 1, -(m_zNear + m_zFar) / 2.f},
		{0, 0, 0, 1.f} };

	M_ortho = {
		{2.f / (xRight - xLeft), 0, 0, 0},
		{0, 2.f / (yTop - yBottom), 0, 0},
		{0, 0, 2.f / (m_zNear - m_zFar), 0 },
		{0, 0, 0, 1.f} };

	m_orthographicMatrix = M_ortho * M_trans;
	// Games101 透视矩阵顺序 Ortho * Trans * Persp
	m_perspectiveMatrix = M_ortho * M_trans * M_persp;
}
