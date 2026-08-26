#include "modeltransform.h"
#include "geometry.h"
#include <Eigen/Geometry>
#include <cmath>

ModelTransform::ModelTransform()
{
    reset();
}

void ModelTransform::reset()
{
    m_translate.setZero();
    m_rotation = Eigen::Quaternionf::Identity();
    m_scale = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
    m_isDirty = true;
}

void ModelTransform::translate(float x, float y, float z)
{
    m_translate += Eigen::Vector3f{ x,y,z };
    m_isDirty = true;
}

void ModelTransform::translate(const Eigen::Vector3f& offset)
{
    m_translate += offset;
    m_isDirty = true;
}

void ModelTransform::rotateX(float angleDeg)
{
    float rad = angleDeg * static_cast<float>(MY_PI) / 180.0f;
    Eigen::Quaternionf q(Eigen::AngleAxisf(rad, Eigen::Vector3f::UnitX()));
    m_rotation = q * m_rotation;
    m_isDirty = true;
}

void ModelTransform::rotateY(float angleDeg)
{
    float rad = angleDeg * static_cast<float>(MY_PI) / 180.0f;
    Eigen::Quaternionf q(Eigen::AngleAxisf(rad, Eigen::Vector3f::UnitY()));
    m_rotation = q * m_rotation;
    m_isDirty = true;
}

void ModelTransform::rotateZ(float angleDeg)
{
    float rad = angleDeg * static_cast<float>(MY_PI) / 180.0f;
    Eigen::Quaternionf q(Eigen::AngleAxisf(rad, Eigen::Vector3f::UnitZ()));
    m_rotation = q * m_rotation;
    m_isDirty = true;
}

void ModelTransform::rotateEuler(float rollDeg, float pitchDeg, float yawDeg)
{
    float roll = rollDeg * static_cast<float>(MY_PI) / 180.0f;
    float pitch = pitchDeg * static_cast<float>(MY_PI) / 180.0f;
    float yaw = yawDeg * static_cast<float>(MY_PI) / 180.0f;

  
    Eigen::Quaternionf q_yaw(Eigen::AngleAxisf(yaw, Eigen::Vector3f::UnitZ()));
    Eigen::Quaternionf q_pitch(Eigen::AngleAxisf(pitch, Eigen::Vector3f::UnitY()));
    Eigen::Quaternionf q_roll(Eigen::AngleAxisf(roll, Eigen::Vector3f::UnitX()));

    // 顺序 Z * Y * X，和原来逻辑保持一致
    m_rotation = q_yaw * q_pitch * q_roll;
    m_isDirty = true;
}


void ModelTransform::scale(float uniformScale)
{
   // m_scale *= Eigen::Vector3f{ uniformScale, uniformScale, uniformScale };
    m_scale *= uniformScale;
    m_isDirty = true;
}

void ModelTransform::scale(float sx, float sy, float sz)
{
    m_scale.x() *= sx;
    m_scale.y() *= sy;
    m_scale.z() *= sz;
    m_isDirty = true;
}

void ModelTransform::scale(const Eigen::Vector3f& s)
{
    m_scale.x() *= s.x();
    m_scale.y() *= s.y();
    m_scale.z() *= s.z();
    m_isDirty = true;
}

void ModelTransform::updateMatrix() const
{
    if (!m_isDirty)
        return;

    // S：缩放矩阵
    Eigen::Matrix4f matS = Eigen::Matrix4f::Identity();
    matS(0, 0) = m_scale.x();
    matS(1, 1) = m_scale.y();
    matS(2, 2) = m_scale.z();

    // R：四元数转旋转矩阵
    Eigen::Matrix4f matR = Eigen::Matrix4f::Identity();
    matR.block<3, 3>(0, 0) = m_rotation.toRotationMatrix();

    // T：平移矩阵
    Eigen::Matrix4f matT = Eigen::Matrix4f::Identity();
    matT.block<3, 1>(0, 3) = m_translate;

    // Model = T * R * S
    m_cachedModelMatrix = matT * matR * matS;
    m_isDirty = false;
}

Eigen::Matrix4f ModelTransform::getModelMatrix() const
{
    updateMatrix();
    return m_cachedModelMatrix;
}
