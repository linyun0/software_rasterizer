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
    void SetCamera(const glm::vec3& position, const glm::vec3& target,const glm::vec3& up=glm::vec3{0,1,0});
    void RotateAroundTarget(glm::vec2 motion);
    void MoveTarget(glm::vec2 motion);
    void CloseToTarget(int ratio);
    void SetModel(glm::vec3 modelCentre, float yRange);
    glm::mat4x4 GetLookAtMatrix();
    glm::mat4x4 GetPerspectiveMatrix();
    glm::mat4x4 GetOrthographicMatrix();
private:
    void GenerateLookAtMatrix();
    void GenerateProjectionMatrix();
private:
    float m_aspect;
    glm::vec3 m_position;
    glm::vec3  m_target;
    glm::vec3 m_updirection{ 0,1,0 };
    float m_zNear;
    float m_zFar;
    float m_zfov;

    glm::mat4x4 m_lookAtMatrix;
    glm::mat4x4 m_perspectiveMatrix;
    glm::mat4x4 m_orthographicMatrix;
};

