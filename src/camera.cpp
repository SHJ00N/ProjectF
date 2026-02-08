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
    glm::vec3 pivot = targetPos + glm::vec3(0.0f, 1.5f, 0.0f);

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
    // if(pitch > 10.0f)
    // {
    //     fov = glm::mix(fov, 25.0f, dt * 6.0f);
    // } 
    // else 
    // {
    //     fov = glm::mix(fov, targetFov, dt * 6.0f);
    // }
    fov = glm::mix(fov, targetFov, dt * 6.0f);
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

    if(targetFov < 25.0f) targetFov = 25.0f;
    if(targetFov > 45.0f) targetFov = 45.0f;
}

Frustum Camera::GetCameraFrustum(float width, float height)
{
    float aspect = width / height;
	const float halfVSide = farPlane * tanf(fov * 0.5f);
	const float halfHSide = halfVSide * aspect;
	const glm::vec3 frontMultFar = farPlane * cameraFront;

	frustum.nearFace = { cameraPos + nearPlane * cameraFront, cameraFront };
	frustum.farFace = { cameraPos + frontMultFar, -cameraFront };
	frustum.rightFace = { cameraPos, glm::cross(frontMultFar - cameraRight * halfHSide, cameraUp) };
	frustum.leftFace = { cameraPos, glm::cross(cameraUp, frontMultFar + cameraRight * halfHSide) };
	frustum.topFace = { cameraPos, glm::cross(cameraRight, frontMultFar - cameraUp * halfVSide) };
	frustum.bottomFace = { cameraPos, glm::cross(frontMultFar + cameraUp * halfVSide, cameraRight) };
	return frustum;
}