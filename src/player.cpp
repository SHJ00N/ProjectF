#include "player.h"

float NormalizeAngle(float angle);

#pragma region lifecycle
Player::Player() : Speed(PLAYER_SPEED)
{
}

Player::Player(Model &model, Shader &shader, glm::vec3 position, glm::vec3 size, glm::vec3 rotation, glm::vec2 velocity) : Speed(PLAYER_SPEED)
{
    ObjectTransform.position = position;
    ObjectTransform.scale = size;
    ObjectTransform.rotation = rotation;
    m_model = &model;
    m_shader = &shader;
}

#pragma endregion

#pragma region game_object_update
void Player::Update(float dt)
{
    // update animator
    UpdateAnimation(dt);
    // update height based on terrain
    updateHeight(dt);
}

void Player::Render()
{
    m_renderer.Draw(*m_shader, ObjectTransform, *m_model, m_animator3D);
}

void Player::RenderShadow()
{
    m_renderer.DrawShadow(ObjectTransform, *m_model, m_animator3D);
}
void Player::UpdateAnimation(float dt)
{
    m_animator3D.UpdateAnimation(dt);
}
void Player::SetAnimation(Animation* animation)
{
    m_animator3D.PlayAnimation(animation);
}

void Player::updateHeight(float dt)
{
    float yDiff = m_terrainHeight - ObjectTransform.position.y;
    // set object height
    // if(yDiff > 0.0f)
    // {
    //     ObjectTransform.position.y = m_terrainHeight;
    // }
    // else
    // {
    //     ObjectTransform.position.y = glm::mix(
    //         ObjectTransform.position.y,
    //         m_terrainHeight,
    //         10.0f * dt
    //     );
    // }
    ObjectTransform.position.y = m_terrainHeight;
}

#pragma endregion

#pragma region player_controls
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
    glm::vec3 projectedMove;
    if(chunk){
        glm::vec3 normal = chunk->GetWorldNormal(ObjectTransform.position.x, ObjectTransform.position.z);
        projectedMove = glm::normalize(moveDir - normal * glm::dot(moveDir, normal));
    }

    // set camera distance
    float dot = glm::dot(moveDir, frontXZ);
    if(dot < 0.0f) camera.targetDistance = BACK_DISTANCE;
    else camera.targetDistance = NORMAL_DISTANCE;

    // result of atan2 is radians, convert radinas to degrees
    float targetYaw = glm::degrees(atan2(moveDir.x, moveDir.z));

    float delta = NormalizeAngle(targetYaw - ObjectTransform.rotation.y);
    ObjectTransform.rotation.y += delta * dt * 10.0f;

    // move
    ObjectTransform.position += (chunk ? projectedMove : glm::normalize(glm::vec3(1.0f))) * velocity;
    if(chunk) SetTerrainHeight(chunk->GetWorldHeight(ObjectTransform.position.x, ObjectTransform.position.z));
    updateHeight(dt);
}

float NormalizeAngle(float angle)
{
    while(angle > 180.0f) angle -= 360.0f;
    while(angle < -180.0f) angle += 360.0f;
    return angle;
}

#pragma endregion