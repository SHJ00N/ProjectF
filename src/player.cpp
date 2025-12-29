#include "player.h"

float NormalizeAngle(float angle);

Player::Player() : GameObject(), Speed(PLAYER_SPEED)
{
}

Player::Player(Model &model, glm::vec3 position, glm::vec3 size, glm::vec3 rotation, glm::vec2 velocity)
    : GameObject(model, position, size, rotation, velocity), Speed(PLAYER_SPEED)
{
}

void Player::UpdateHeight(float terrainHeight, float dt)
{
    float yDiff = terrainHeight - ObjectTransform.position.y;
    // set object height
    if(yDiff > 0.0f)
    {
        ObjectTransform.position.y = terrainHeight;
    }
    else
    {
        ObjectTransform.position.y = glm::mix(
            ObjectTransform.position.y,
            terrainHeight,
            10.0f * dt
        );
    }
}

void Player::Move(Camera_Movement direction, Camera &camera, Chunk *chunk, float dt)
{
    float velocity = Speed * dt;

    // init input state
    float inputForward = 0.0f;
    float inputRight = 0.0f;

    glm::vec3 frontXZ = glm::normalize(glm::vec3(camera.cameraFront.x, 0.0f, camera.cameraFront.z));
    glm::vec3 rightXZ = glm::normalize(glm::cross(frontXZ, camera.worldUp));

    // set input state
    if(direction == FORWARD)    inputForward += 1.0f;
    if(direction == BACKWARD)   inputForward -= 1.0f;
    if(direction == RIGHT)      inputRight   += 1.0f;
    if(direction == LEFT)       inputRight   -= 1.0f;

    glm::vec3 moveDir = frontXZ * inputForward + rightXZ * inputRight;
    moveDir = glm::normalize(moveDir);

    // project moveDir to ground normal
    glm::vec3 normal = chunk->GetWorldNormal(ObjectTransform.position.x, ObjectTransform.position.z);
    glm::vec3 projectedMove = glm::normalize(moveDir - normal * glm::dot(moveDir, normal));

    // set camera distance
    float dot = glm::dot(moveDir, frontXZ);
    if(dot < 0.0f) camera.targetDistance = BACK_DISTANCE;
    else camera.targetDistance = NORMAL_DISTANCE;

    // result of atan2 is radians, convert radinas to degrees
    float targetYaw = glm::degrees(atan2(moveDir.x, moveDir.z));

    float delta = NormalizeAngle(targetYaw - ObjectTransform.rotation.y);
    ObjectTransform.rotation.y += delta * dt * 10.0f;

    // move
    ObjectTransform.position += projectedMove * velocity;
    UpdateHeight(chunk->GetWorldHeight(ObjectTransform.position.x, ObjectTransform.position.z), dt);
}

float NormalizeAngle(float angle)
{
    while(angle > 180.0f) angle -= 360.0f;
    while(angle < -180.0f) angle += 360.0f;
    return angle;
}