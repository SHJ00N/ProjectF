#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch){
    cameraPos = position;
    worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    sensitivity = SENSITIVITY;
    movementSpeed = MOVESPEED;
    fov = FOV;

    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch){
    cameraPos = glm::vec3(posX, posY, posZ);
    worldUp = glm::vec3(upX, upY, upZ);
    this->yaw = yaw;
    this->pitch = pitch;
    sensitivity = SENSITIVITY;
    movementSpeed = MOVESPEED;
    fov = FOV;

    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix(){
    return glm::lookAt(cameraPos, cameraPos + cameraFront, worldUp);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime){
    float velocity = movementSpeed * deltaTime;
    if(direction == FORWARD) cameraPos += cameraFront * velocity;
    if(direction == BACKWARD) cameraPos -= cameraFront * velocity;
    if(direction == RIGHT) cameraPos += cameraRight * velocity;
    if(direction == LEFT) cameraPos -= cameraRight * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset){
    yaw += xoffset * sensitivity;
    pitch += yoffset * sensitivity;

    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset){
    fov -= yoffset;
    if(fov < 1.0f) fov = 1.0f;
    if(fov > 45.0f) fov = 45.0f;
}

void Camera::updateCameraVectors(){
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

    cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}