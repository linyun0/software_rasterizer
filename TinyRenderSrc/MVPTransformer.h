#pragma once
#include <Eigen/Eigen>
#include "../TinyRenderSrc/geometry.h"


class MVPTransformer
{

public:
    MVPTransformer();

    Eigen::Matrix4f get_view_matrix()
    {
        return ViewMatrix;
    }

    Eigen::Matrix4f get_model_matrix()
    {
        return modelMatrix;
    }

    Eigen::Matrix4f get_projection_matrix()
    {
        return ProjectionMatrix;

    }
    
    void SetCameraMove(float angle, Eigen::Vector3f step);
    void SetModelArc(float angle,float scale);
    void SetProjectionArc(float eye_fov,float aspect_ratio, float zNear, float zFar);

private:
    Eigen::Vector3f m_eye_pos={0,0,10}, m_eye_lookat, m_eye_up;
    float m_angle=140, m_scalecoef=2;
    float m_eye_fov=45, m_aspect_ratio=1, m_zNear=0.1, m_zFar=50;

    Eigen::Matrix4f modelMatrix, ViewMatrix, ProjectionMatrix;
};
