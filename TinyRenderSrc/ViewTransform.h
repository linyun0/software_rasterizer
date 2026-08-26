#pragma once
#include <Eigen/Dense>

/**
 * @brief 视图变换，相机控制
 * 两种工作模式：
 * 1. LookAt模式：相机位置 + 目标看点 + 上向量
 * 2. 欧拉角模式：位置 + yaw(偏航) pitch(俯仰)，适合FPS第一人称
 * getViewMatrix() 返回 View矩阵
 */
class ViewTransform
{
public:
    ViewTransform();

    //======== LookAt 模式接口 =========
    void setLookAt(const Eigen::Vector3f& cameraPos,
        const Eigen::Vector3f& target,
        const Eigen::Vector3f& up = Eigen::Vector3f::UnitY());

    //======== FPS 欧拉角模式（yaw偏航，pitch俯仰，角度制）========
    void setPosition(const Eigen::Vector3f& pos);
    void setYawPitch(float yawDeg, float pitchDeg);
    void rotateYaw(float deltaYawDeg);
    void rotatePitch(float deltaPitchDeg);

    // 相机前后左右移动（沿相机局部坐标系）
    void moveForward(float dist);
    void moveRight(float dist);
    void moveUp(float dist);

    // 重置相机
    void reset();

    // 获取 View矩阵
    Eigen::Matrix4f getViewMatrix() const;

    // 调试：获取相机参数
    Eigen::Vector3f getCameraPosition() const;
    Eigen::Vector3f getCameraFront() const;

private:
    void updateView() const;

    // 模式1：LookAt参数
    Eigen::Vector3f m_camPos;
    Eigen::Vector3f m_target;
    Eigen::Vector3f m_up;

    // 模式2：FPS欧拉角
    float m_yawDeg;
    float m_pitchDeg;

    // 缓存与脏标记
    mutable Eigen::Matrix4f m_cachedView;
    mutable bool m_isDirty;
};
