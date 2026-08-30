#pragma once
#include <Eigen/Dense>

/**
 * @brief 投影变换类
 * 支持透视投影 / 正交投影
 * 构造传入透视参数 fov(角度), aspect_ratio, zNear, zFar
 * 可切换模式，分别获取透视矩阵、正交矩阵
 * 注意：遵循 OpenGL 右手规则，NDC：x[-1,1] y[-1,1] z[-1,1]
 */
class ProjectionTransform
{
public:
    /// @param eye_fov 透视视场角，角度制
    /// @param aspect_ratio 宽高比 width / height
    /// @param zNear 近裁剪面 (>0)
    /// @param zFar 远裁剪面 (> zNear)
    ProjectionTransform(float eye_fov, float aspect_ratio, float zNear, float zFar);
    void SetAspect(float in_aspect_ratio);
    // 更新透视投影参数
    void setPerspectiveParam(float eye_fov, float aspect_ratio, float zNear, float zFar);

    // 获取透视投影矩阵 (Perspective)
    Eigen::Matrix4f getPerspectiveMatrix() const;

    /// 正交投影
    /// @param left right bottom top 裁剪盒边界
    /// @param zNear zFar 深度裁剪
    Eigen::Matrix4f getOrthoMatrix(float left, float right,
        float bottom, float top,
        float zNear, float zFar) const;

private:
    float m_fovDeg;
    float m_aspect;
    float m_zNear;
    float m_zFar;
};
