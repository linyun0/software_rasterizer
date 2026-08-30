#include "ProjectionTransform.h"
#include <cmath>
#include "geometry.h"
namespace
{
    constexpr float DEG2RAD(float deg)
    {
        return deg * static_cast<float>(MY_PI) / 180.0f;
    }
}

ProjectionTransform::ProjectionTransform(float eye_fov, float aspect_ratio, float zNear, float zFar)
    : m_fovDeg(eye_fov)
    , m_aspect(aspect_ratio)
    , m_zNear(zNear)
    , m_zFar(zFar)
{
}

void ProjectionTransform::SetAspect(float in_aspect_ratio) {
    m_aspect = in_aspect_ratio;
}

void ProjectionTransform::setPerspectiveParam(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    m_fovDeg = eye_fov;
    m_aspect = aspect_ratio;
    m_zNear = zNear;
    m_zFar = zFar;
}

// OpenGL风格透视投影矩阵，输出裁剪空间矩阵
Eigen::Matrix4f ProjectionTransform::getPerspectiveMatrix() const
{
    Eigen::Matrix4f mat = Eigen::Matrix4f::Zero();

    float rad = DEG2RAD(m_fovDeg);
    float tanHalfFov = std::tan(rad / 2.0f);

    mat(0, 0) = 1.0f / (m_aspect * tanHalfFov);
    mat(1, 1) = 1.0f / tanHalfFov;
    mat(2, 2) = -(m_zFar + m_zNear) / (m_zFar - m_zNear);
    mat(2, 3) = -(2.0f * m_zFar * m_zNear) / (m_zFar - m_zNear);
    mat(3, 2) = -1.0f;

    return mat;
}

Eigen::Matrix4f ProjectionTransform::getOrthoMatrix(float left, float right,
    float bottom, float top,
    float zNear, float zFar) const
{
    Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();

    mat(0, 0) = 2.0f / (right - left);
    mat(1, 1) = 2.0f / (top - bottom);
    mat(2, 2) = -2.0f / (zFar - zNear);

    mat(0, 3) = -(right + left) / (right - left);
    mat(1, 3) = -(top + bottom) / (top - bottom);
    mat(2, 3) = -(zFar + zNear) / (zFar - zNear);

    return mat;
}
