#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch){
    cameraPos = position;
    worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    sensitivity = SENSITIVITY;
    distance = NORMAL_DISTANCE;
    targetDistance = NORMAL_DISTANCE;
    fov = FOV;
    targetFov = FOV;
    followYawOffset = 0.0f;
    nearPlane = NEAR_PLANE;
    farPlane = FAR_PLANE;
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch){
    cameraPos = glm::vec3(posX, posY, posZ);
    worldUp = glm::vec3(upX, upY, upZ);
    this->yaw = yaw;
    this->pitch = pitch;
    sensitivity = SENSITIVITY;
    distance = NORMAL_DISTANCE;
    targetDistance = NORMAL_DISTANCE;
    fov = FOV;
    targetFov = FOV;
    followYawOffset = 0.0f;
    nearPlane = NEAR_PLANE;
    farPlane = FAR_PLANE;
}

void Camera::Update(const glm::vec3 &targetPos, float dt){
    glm::vec3 pivot = targetPos + glm::vec3(0.0f, 0.5f, 0.0f);

    float finalYaw = yaw + followYawOffset;

    glm::vec3 direction;
    direction.x = cos(glm::radians(pitch)) * sin(glm::radians(finalYaw));
    direction.y = sin(glm::radians(pitch));
    direction.z = cos(glm::radians(pitch)) * cos(glm::radians(finalYaw));
    direction = glm::normalize(direction);

    // Update camera position to target position minus offset
    distance = glm::mix(distance, targetDistance, dt * 6.0f);
    cameraPos = pivot - direction * distance;

    cameraFront = glm::normalize(pivot - cameraPos);
    cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
    cameraUp    = glm::normalize(glm::cross(cameraRight, cameraFront)); 

    // zoom when the pitch angle is low
    if(pitch > 10.0f)
    {
        fov = glm::mix(fov, 30.0f, dt * 6.0f);
    } 
    else 
    {
        fov = glm::mix(fov, targetFov, dt * 6.0f);
    }
}

glm::mat4 Camera::GetViewMatrix(){
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset){
    followYawOffset -= xoffset * sensitivity;
    pitch += yoffset * sensitivity;

    if(pitch > 25.0f) pitch = 25.0f;
    if(pitch < -45.0f) pitch = -45.0f;
}

glm::mat4 Camera::GetProjectionMatrix(float width, float height)
{
    return glm::perspective(glm::radians(fov), width / height, NEAR_PLANE, FAR_PLANE);
}

void Camera::ProcessMouseScroll(float yoffset){
    targetFov -= yoffset;

    if(targetFov < 30.0f) targetFov = 30.0f;
    if(targetFov > 45.0f) targetFov = 45.0f;
}

Frustum Camera::GetCameraFrustum(float width, float height)
{
    // calculate viewProjection matrix
    glm::mat4 viewMat = GetViewMatrix();
    glm::mat4 projMat = GetProjectionMatrix(width, height);

    glm::mat4 vpMat = projMat * viewMat;

    // near/far plane
    frustum.nearFace = { cameraPos + nearPlane * cameraFront, cameraFront };
    frustum.farFace = { cameraPos + farPlane * cameraFront, -cameraFront };
    // left/right/top/bottom plane
    frustum.leftFace = { cameraPos, glm::vec3(vpMat[0][3] + vpMat[0][0], vpMat[1][3] + vpMat[1][0], vpMat[2][3] + vpMat[2][0]) };
    frustum.rightFace = { cameraPos, glm::vec3(vpMat[0][3] - vpMat[0][0], vpMat[1][3] - vpMat[1][0], vpMat[2][3] - vpMat[2][0]) };
    frustum.topFace = { cameraPos, glm::vec3(vpMat[0][3] - vpMat[0][1], vpMat[1][3] - vpMat[1][1], vpMat[2][3] - vpMat[2][1]) };
    frustum.bottomFace = { cameraPos, glm::vec3(vpMat[0][3] + vpMat[0][1], vpMat[1][3] + vpMat[1][1], vpMat[2][3] + vpMat[2][1]) };

    return frustum;
}