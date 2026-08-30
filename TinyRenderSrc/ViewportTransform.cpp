#include "ViewportTransform.h"

ViewportTransform::ViewportTransform(int width, int height, float near, float far)
    : m_width(width), m_height(height), m_near(near), m_far(far)
{
}

void ViewportTransform::SetWidth(const int& in_width)  {
    m_width = in_width;
}
void ViewportTransform::SetHeight(const int& in_height) {
    m_height = in_height;
}
float ViewportTransform::GetNear() const {
    return m_near;
}
float ViewportTransform::GetFar() const {
    return  m_far;
}

Eigen::Matrix4f ViewportTransform::GetMatrix() const
{
    Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();

    float w2 = static_cast<float>(m_width) / 2.0f;
    float h2 = static_cast<float>(m_height) / 2.0f;
    float f1 = (m_far - m_near) / 2.0f;
    float f2 = (m_far + m_near) / 2.0f;

    // ÁÐÖ÷Ðò¸³Öµ
    mat(0, 0) = w2;     mat(0, 3) = w2;
    mat(1, 1) = -h2;    mat(1, 3) = h2;
    mat(2, 2) = f1;     mat(2, 3) = f2;

    return mat;
}

Eigen::Matrix4f ViewportTransform::GetOpenGLStandardMatrix() const
{
    Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();
    float w2 = static_cast<float>(m_width) / 2.0f;
    float h2 = static_cast<float>(m_height) / 2.0f;

    mat(0, 0) = w2;     mat(0, 3) = w2;
    mat(1, 1) = -h2;    mat(1, 3) = h2;
    mat(2, 2) = 0.5f;   mat(2, 3) = 0.5f;

    return mat;
}

void ViewportTransform::Apply(Eigen::Vector4f& vec) const
{
    Eigen::Matrix4f vp = GetMatrix();
    vec = vp * vec;
}

void ViewportTransform::Resize(int width, int height)
{
    m_width = width;
    m_height = height;
}

void ViewportTransform::SetNearFar(float near, float far)
{
    m_near = near;
    m_far = far;
}
