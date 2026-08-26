#pragma once
#include <Eigen/Dense>
#include <Eigen/Geometry>
/**
 * @brief 模型变换类：平移、旋转、缩放，维护model矩阵
 * 变换顺序：缩放 S → 旋转 R → 平移 T
 * final_matrix = T * R * S
 */
class ModelTransform
{
public:
  

    ModelTransform();
    // 平移：局部/世界轴平移
    void translate(float x, float y, float z);
    void translate(const Eigen::Vector3f& offset);
    // 旋转：绕 X/Y/Z 轴，angle 单位 角度(deg)
    void rotateX(float angleDeg);
    void rotateY(float angleDeg);
    void rotateZ(float angleDeg);
    // 欧拉角旋转 (roll,pitch,yaw) 角度制
    void rotateEuler(float rollDeg, float pitchDeg, float yawDeg);
    // 缩放
    void scale(float uniformScale);             // 均匀缩放
    void scale(float sx, float sy, float sz);   // 非均匀缩放
    void scale(const Eigen::Vector3f& s);
    // 重置：回到单位矩阵
    void reset();
    // 获取最终组合好的 Model 变换矩阵 T*R*S
    Eigen::Matrix4f getModelMatrix() const;
private:
    // 保存分解形式，而不是直接保存一个矩阵；方便增量修改
    Eigen::Vector3f m_translate;
    Eigen::Quaternionf m_rotation;
    Eigen::Vector3f m_scale;
    // 缓存计算出来的矩阵；标记是否脏，避免重复计算
    mutable Eigen::Matrix4f m_cachedModelMatrix;
    mutable bool m_isDirty;
    // 内部重新计算矩阵
    void updateMatrix() const;
};
