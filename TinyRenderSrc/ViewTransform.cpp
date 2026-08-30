#include "ViewTransform.h"
#include <Eigen/Geometry>
#include <cmath>
#include <Eigen/Dense>   // ?必须要有

#include "geometry.h"
#ifndef MY_PI
#define MY_PI 3.14159265358979323846f
#endif

namespace {
    constexpr float DEG2RAD(float deg)
    {
        return deg * static_cast<float>(MY_PI) / 180.0f;
    }
}

ViewTransform::ViewTransform()
{
    // 默认：相机在(0,0,3)看向原点
   // setLookAt(Eigen::Vector3f(0, 0, 3), Eigen::Vector3f(0, 0, 0), Eigen::Vector3f::UnitY());
}

void ViewTransform::setLookAt(const Eigen::Vector3f& cameraPos,
    const Eigen::Vector3f& target,
    const Eigen::Vector3f& up)
{
    m_camPos = cameraPos;
    m_target = target;
    m_up = up;
}

Eigen::Matrix4f ViewTransform::computeLookAtMatrix() const
{
    Eigen::Vector3f z = (m_camPos - m_target).normalized();
    Eigen::Vector3f x = (m_up.cross(z)).normalized();
    Eigen::Vector3f y = z.cross(x);

    Eigen::Matrix3f R3;
    R3.col(0) = x;
    R3.col(1) = y;
    R3.col(2) = z;

    Eigen::Matrix4f R = Eigen::Matrix4f::Identity();
    R.block<3, 3>(0, 0) = R3;

    Eigen::Matrix4f T = Eigen::Matrix4f::Identity();
    T.block<3, 1>(0, 3) = -m_camPos;

    Eigen::Matrix4f view = R.transpose() * T;
    return view;
}

Eigen::Matrix4f ViewTransform::getViewMatrix() const
{
    // 每一次：拿最新 m_camPos/m_target/m_up，重新生成lookAt矩阵
    return computeLookAtMatrix();
}

void ViewTransform::translateWorld(const Eigen::Vector3f& delta)
{
    // 构造世界平移矩阵
    Eigen::Affine3f trans = Eigen::Affine3f::Identity();
    trans.translate(delta);

    // 矩阵变换更新世界坐标
    m_camPos = trans * m_camPos;
    m_target = trans * m_target;
}

void ViewTransform::rotateAroundTargetByMatrix(const Eigen::Vector3f& axis, float angleDeg)
{
    ////float rad = DEG2RAD(angleDeg);
    //// 1.构造旋转矩阵
    //Eigen::Matrix4f rotation;
    //float count_angle = angleDeg * MY_PI / 180.f;
    //rotation << cos(count_angle), 0, sin(count_angle), 0,
    //    0, 1, 0, 0,
    //    -sin(count_angle), 0, cos(count_angle), 0,
    //    0, 0, 0, 1;

    //// 2.相机相对于target的偏移向量
    //Eigen::Vector3f offset = m_camPos - m_target;
    ////offset.normalize();

    //// 3.用旋转矩阵变换偏移向量
    //Eigen::Vector3f newOffset = rotation * offset;

    ////4.回写相机世界位置
    //m_camPos = m_target + newOffset;
}

void ViewTransform::moveAlongViewDirByMatrix(float dist)
{
    Eigen::Vector3f viewDir = (m_camPos - m_target).normalized();
    Eigen::Affine3f trans = Eigen::Affine3f::Identity();
    trans.translate(viewDir * dist);

    // 只移动相机位置，target不动
    m_camPos = trans * m_camPos;
}

void ViewTransform::panCameraByMatrix(float rightDist, float upDist)
{
    Eigen::Vector3f z = (m_camPos - m_target).normalized();
    Eigen::Vector3f right = (m_up.cross(z)).normalized();
    Eigen::Vector3f camUp = z.cross(right);

    Eigen::Vector3f delta = right * rightDist + camUp * upDist;
    Eigen::Affine3f trans = Eigen::Affine3f::Identity();
    trans.translate(delta);

    // pan：相机和目标一起平移
    m_camPos = trans * m_camPos;
    m_target = trans * m_target;
}

void ViewTransform::SetPos(const Eigen::Vector3f& pos) {
    m_camPos = pos;
    return;
}

void ViewTransform::rotateAroundY(float angleDeg)
{
    float rad = angleDeg * MY_PI / 180.0f;

    Eigen::Matrix4f rotY;
    rotY <<
        std::cos(rad), 0, std::sin(rad), 0,
        0, 1, 0, 0,
        -std::sin(rad), 0, std::cos(rad), 0,
        0, 0, 0, 1;

    Eigen::Vector3f offset = m_camPos - m_target;
    Eigen::Vector4f offsetH(offset.x(), offset.y(), offset.z(), 1.0f);

    Eigen::Vector4f newOffsetH = rotY * offsetH;
    Eigen::Vector3f newOffset(newOffsetH.x(), newOffsetH.y(), newOffsetH.z());

    m_camPos = m_target + newOffset;
}

void ViewTransform::rotateAroundX(float angleDeg)
{
    float rad = angleDeg * MY_PI / 180.0f;

    // 右手系 绕世界X轴 4×4旋转矩阵
    Eigen::Matrix4f rotX;
    rotX <<
        1, 0, 0, 0,
        0, std::cos(rad), -std::sin(rad), 0,
        0, std::sin(rad), std::cos(rad), 0,
        0, 0, 0, 1;

    Eigen::Vector3f offset = m_camPos - m_target;
    Eigen::Vector4f offsetH(offset.x(), offset.y(), offset.z(), 1.0f);

    Eigen::Vector4f newOffsetH = rotX * offsetH;
    Eigen::Vector3f newOffset(newOffsetH.x(), newOffsetH.y(), newOffsetH.z());

    m_camPos = m_target + newOffset;
}