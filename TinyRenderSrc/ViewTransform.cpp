#include "ViewTransform.h"
#include <Eigen/Geometry>
#include "geometry.h"
#include <cmath>

namespace {
    constexpr float DEG2RAD(float deg)
    {
        return deg * static_cast<float>(MY_PI) / 180.0f;
    }
}

ViewTransform::ViewTransform()
{
    reset();
}

void ViewTransform::reset()
{
    // 默认相机：放在(0,0,3)看向原点
    m_camPos = Eigen::Vector3f(0, 0, 3.0f);
    m_target = Eigen::Vector3f(0, 0, 0);
    m_up = Eigen::Vector3f::UnitY();

    m_yawDeg = -90.0f;   // 默认看向-Z
    m_pitchDeg = 0.0f;

    m_isDirty = true;
}

void ViewTransform::setLookAt(const Eigen::Vector3f& cameraPos,
    const Eigen::Vector3f& target,
    const Eigen::Vector3f& up)
{
    m_camPos = cameraPos;
    m_target = target;
    m_up = up;
    m_isDirty = true;
}

void ViewTransform::setPosition(const Eigen::Vector3f& pos)
{
    m_camPos = pos;
    m_isDirty = true;
}

void ViewTransform::setYawPitch(float yawDeg, float pitchDeg)
{
    m_yawDeg = yawDeg;
    m_pitchDeg = pitchDeg;
    // 限制俯仰角，防止相机翻转
    if (m_pitchDeg > 89.0f) m_pitchDeg = 89.0f;
    if (m_pitchDeg < -89.0f) m_pitchDeg = -89.0f;
    m_isDirty = true;
}

void ViewTransform::rotateYaw(float deltaYawDeg)
{
    m_yawDeg += deltaYawDeg;
    m_isDirty = true;
}

void ViewTransform::rotatePitch(float deltaPitchDeg)
{
    m_pitchDeg += deltaPitchDeg;
    if (m_pitchDeg > 89.0f)  m_pitchDeg = 89.0f;
    if (m_pitchDeg < -89.0f) m_pitchDeg = -89.0f;
    m_isDirty = true;
}

Eigen::Vector3f ViewTransform::getCameraFront() const
{
    float yawRad = DEG2RAD(m_yawDeg);
    float pitchRad = DEG2RAD(m_pitchDeg);

    Eigen::Vector3f front;
    front.x() = std::cos(pitchRad) * std::cos(yawRad);
    front.y() = std::sin(pitchRad);
    front.z() = std::cos(pitchRad) * std::sin(yawRad);
    return front.normalized();
}

void ViewTransform::moveForward(float dist)
{
    Eigen::Vector3f front = getCameraFront();
    m_camPos += front * dist;
    m_isDirty = true;
}

void ViewTransform::moveRight(float dist)
{
    Eigen::Vector3f front = getCameraFront();
    Eigen::Vector3f right = front.cross(Eigen::Vector3f::UnitY()).normalized();
    m_camPos += right * dist;
    m_isDirty = true;
}

void ViewTransform::moveUp(float dist)
{
    m_camPos += Eigen::Vector3f::UnitY() * dist;
    m_isDirty = true;
}

void ViewTransform::updateView() const
{
    if (!m_isDirty) return;

    // 使用FPS yaw/pitch生成LookAt
    Eigen::Vector3f front = getCameraFront();
    Eigen::Vector3f target = m_camPos + front;

    // Eigen::LookAt 生成view矩阵
    m_cachedView = Eigen::Matrix4f::Identity();
    Eigen::Vector3f right = front.cross(Eigen::Vector3f::UnitY()).normalized();
    Eigen::Vector3f up = right.cross(front).normalized();

    Eigen::Matrix3f R;
    R.col(0) = right;
    R.col(1) = up;
    R.col(2) = -front;

    Eigen::Matrix4f rotation = Eigen::Matrix4f::Identity();
    rotation.block<3, 3>(0, 0) = R;

    Eigen::Matrix4f translate = Eigen::Matrix4f::Identity();
    translate.block<3, 1>(0, 3) = -m_camPos;

    // View = R * T
    m_cachedView = rotation * translate;

    m_isDirty = false;
}

Eigen::Matrix4f ViewTransform::getViewMatrix() const
{
    updateView();
    return m_cachedView;
}

Eigen::Vector3f ViewTransform::getCameraPosition() const
{
    return m_camPos;
}
