#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement{
    LEFT, RIGHT, FORWARD, BACKWARD
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;
const float NORMAL_DISTANCE = 5.0f;
const float BACK_DISTANCE = 3.5f;
const float NEAR_PLANE = 1.0f;
const float FAR_PLANE = 3000.0f;

class Camera
{
public:
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraRight;
    glm::vec3 cameraUp;
    glm::vec3 worldUp;

    float yaw;
    float pitch;
    float followYawOffset;

    float distance;
    float targetDistance;
    float sensitivity;
    float fov;
    float targetFov;
    float nearPlane;
    float farPlane;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw = YAW, float pitch = PITCH);

    void Update(glm::vec3 &targetPos,float dt);
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(float width, float height);

    void ProcessMouseMovement(float xoffset, float yoffset);
    void ProcessMouseScroll(float yoffset);
};
#endif