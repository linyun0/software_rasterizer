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

    void setLookAt(const Eigen::Vector3f& cameraPos,
        const Eigen::Vector3f& target,
        const Eigen::Vector3f& up);

    Eigen::Matrix4f getViewMatrix() const;

    // 世界空间整体平移
    void translateWorld(const Eigen::Vector3f& delta);

    /// 绕 target点旋转：构造旋转矩阵，变换相机相对于target的偏移
    void rotateAroundTargetByMatrix(const Eigen::Vector3f& axis, float angleDeg);

    /// 沿视线方向靠近远离，构造平移矩阵更新camPos
    void moveAlongViewDirByMatrix(float dist);

    /// 画面平移pan，构造平移矩阵同时移动camPos与target
    void panCameraByMatrix(float rightDist, float upDist);

    void SetPos(const Eigen::Vector3f& pos);

        // 绕世界Y轴旋转
        void rotateAroundY(float angleDeg);
    // 绕世界X轴旋转
    void rotateAroundX(float angleDeg);

private:
    Eigen::Matrix4f computeLookAtMatrix() const;

    Eigen::Vector3f m_camPos;
    Eigen::Vector3f m_target;
    Eigen::Vector3f m_up;
};