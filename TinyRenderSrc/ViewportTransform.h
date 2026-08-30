#pragma once
#include <Eigen/Dense>

class ViewportTransform
{
private:
    int   m_width;
    int   m_height;
    float m_near;
    float m_far;

public:
    /**
     * @param width 像素宽
     * @param height 像素高
     * @param near 视锥体近裁剪面(眼空间)
     * @param far  视锥体远裁剪面(眼空间)
     */
    ViewportTransform(int width, int height, float near, float far);
    void SetWidth(const int& width) ;
    void SetHeight(const int& height) ;
    float GetNear() const;
    float GetFar() const;

    // Games101 视口矩阵：
    // X,Y:NDC[-1,1] →屏幕像素
    // Z:NDC[-1,1] → [near, far] 眼空间深度，适配光栅化代码
    Eigen::Matrix4f GetMatrix() const;

    // OpenGL标准视口矩阵 Z:NDC[-1,1] → [0, 1]
    Eigen::Matrix4f GetOpenGLStandardMatrix() const;

    /**
     * @brief 对NDC齐次点做视口变换，输入vec.w() ==1
     * @param vec 输入NDC(x,y,z,w=1)；输出屏幕像素x,y，z为眼空间深度[near,far]
     */
    void Apply(Eigen::Vector4f& vec) const;

    // 窗口resize
    void Resize(int width, int height);

    void SetNearFar(float near, float far);
};
