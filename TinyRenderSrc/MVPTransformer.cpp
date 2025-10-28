#include "MVPTransformer.h"

MVPTransformer::MVPTransformer()
{
    //====================ModelMatrix===========
    Eigen::Matrix4f rotation;
    m_angle = m_angle * MY_PI / 180.f;
    rotation << cos(m_angle), 0, sin(m_angle), 0,
        0, 1, 0, 0,
        -sin(m_angle), 0, cos(m_angle), 0,
        0, 0, 0, 1;



    Eigen::Matrix4f scale;
    scale << m_scalecoef, 0, 0, 0,
        0, m_scalecoef, 0, 0,
        0, 0, m_scalecoef, 0,
        0, 0, 0, 1;

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1;

    modelMatrix =  translate * rotation * scale;

    //====================ViewMatrix
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f rotateY_180 = Eigen::Matrix4f::Identity();
    rotateY_180 <<
        -1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -1, 0,
        0, 0, 0, 1;


    Eigen::Matrix4f rotateX_180 = Eigen::Matrix4f::Identity();
    rotateX_180 <<
        1, 0, 0, 0,
        0, -1, 0, 0,
        0, 0, -1, 0,
        0, 0, 0, 1;


    Eigen::Matrix4f rotateZ_180 = Eigen::Matrix4f::Identity();
    rotateZ_180 <<
        -1, 0, 0, 0,
        0, -1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1;

   // Eigen::Matrix4f translate;
    translate << 1, 0, 0, -m_eye_pos[0],
        0, 1, 0, -m_eye_pos[1],
        0, 0, 1, -m_eye_pos[2],
        0, 0, 0, 1;

    ViewMatrix= translate * view;

    //===============ProjectionMatrix 
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    float eye_angle = m_eye_fov * MY_PI / 180;
    float t, b, l, r;
    t = -m_zNear * tan(eye_angle / 2);//------
    r = t * m_aspect_ratio;
    l = -r;
    b = -t;
    //Eigen::Matrix4f PersToOrth = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f m1;
    Eigen::Matrix4f m2;
    Eigen::Matrix4f m3;
    m1 << m_zNear, 0, 0, 0,
        0, m_zNear, 0, 0,
        0, 0, m_zNear + m_zFar, -m_zNear * m_zFar,
        0, 0, 1, 0;
    m2 << 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, -(m_zNear + m_zFar) / 2,
        0, 0, 0, 1;
    m3 << 2 / (r - l), 0, 0, 0,
        0, 2 / (t - b), 0, 0,
        0, 0, 2 / (m_zNear - m_zFar), 0,
        0, 0, 0, 1;
    ProjectionMatrix= m3 * m2 * m1 * projection;


}

void MVPTransformer::SetCameraMove(float angle, Eigen::Vector3f step)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f rotateY_180 = Eigen::Matrix4f::Identity();
    rotateY_180 <<
        -1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -1, 0,
        0, 0, 0, 1;


    Eigen::Matrix4f rotateX_180 = Eigen::Matrix4f::Identity();
    rotateX_180 <<
        1, 0, 0, 0,
        0, -1, 0, 0,
        0, 0, -1, 0,
        0, 0, 0, 1;


    Eigen::Matrix4f rotateZ_180 = Eigen::Matrix4f::Identity();
    rotateZ_180 <<
        -1, 0, 0, 0,
        0, -1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1;

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -m_eye_pos[0],
        0, 1, 0, -m_eye_pos[1],
        0, 0, 1, -m_eye_pos[2],
        0, 0, 0, 1;

    ViewMatrix = translate * view;
}

void MVPTransformer::SetModelArc(float angle, float scale)
{
    m_angle = angle;
    m_scalecoef =scale;

    Eigen::Matrix4f rotation;
    m_angle = m_angle * MY_PI / 180.f;
    rotation << cos(m_angle), 0, sin(m_angle), 0,
        0, 1, 0, 0,
        -sin(m_angle), 0, cos(m_angle), 0,
        0, 0, 0, 1;

    Eigen::Matrix4f matrix_scale;
    matrix_scale << m_scalecoef, 0, 0, 0,
        0, m_scalecoef, 0, 0,
        0, 0, m_scalecoef, 0,
        0, 0, 0, 1;

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1;

    modelMatrix = translate * rotation *matrix_scale;
}

void MVPTransformer::SetProjectionArc(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    m_eye_fov = eye_fov;
    m_aspect_ratio = aspect_ratio;
    m_zNear = zNear;
    m_zFar = zFar;


    //===============ProjectionMatrix 
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    float eye_angle = m_eye_fov * MY_PI / 180;
    float t, b, l, r;
    t = -m_zNear * tan(eye_angle / 2);//------
    r = t * m_aspect_ratio;
    l = -r;
    b = -t;
    //Eigen::Matrix4f PersToOrth = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f m1;
    Eigen::Matrix4f m2;
    Eigen::Matrix4f m3;
    m1 << m_zNear, 0, 0, 0,
        0, m_zNear, 0, 0,
        0, 0, m_zNear + m_zFar, -m_zNear * m_zFar,
        0, 0, 1, 0;
    m2 << 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, -(m_zNear + m_zFar) / 2,
        0, 0, 0, 1;
    m3 << 2 / (r - l), 0, 0, 0,
        0, 2 / (t - b), 0, 0,
        0, 0, 2 / (m_zNear - m_zFar), 0,
        0, 0, 0, 1;
    ProjectionMatrix = m3 * m2 * m1 * projection;
}
