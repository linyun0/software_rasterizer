#pragma once
#include "geometry.h"
#include <qimage.h>
#include <qsize.h>
enum CameraPara { FOV, NEAR };
//Orbit Camera
class Camera
{
public:
    Camera() {}

    void SetViewFrustum(float fov, float aspect, float near, float far);
    void SetCamera(const vec3& position, const vec3& target,const vec3& up=vec3{0,1,0});
    void RotateAroundTarget(vec2 motion);
    void MoveTarget(vec2 motion);
    void CloseToTarget(int ratio);
    void SetModel(vec3 modelCentre, float yRange);
    mat<4, 4> GetLookAtMatrix();
    mat<4, 4> GetPerspectiveMatrix();
    mat<4, 4> GetOrthographicMatrix();
private:
    void GenerateLookAtMatrix();
    void GenerateProjectionMatrix();
private:
    float m_aspect;
    vec3 m_position;
    vec3  m_target;
    vec3 m_updirection{ 0,1,0 };
    float m_zNear;
    float m_zFar;
    float m_zfov;

    mat<4, 4> m_lookAtMatrix;
    mat<4, 4> m_perspectiveMatrix;
    mat<4, 4> m_orthographicMatrix;
};

