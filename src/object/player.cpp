#include "object/player.h"
#include "world/world.h"
#include "frustum.h"
#include "object/weapon.h"

float NormalizeAngle(float angle);

#pragma region lifecycle
Player::Player(Model &model, Shader &shader, glm::vec3 position, glm::vec3 size, glm::vec3 rotation, glm::vec2 velocity) : Speed(PLAYER_SPEED), m_model(model), m_shader(shader)
{
    // set transform info
    transform.SetLocalPosition(position);
    transform.SetLocalScale(size);
    transform.SetLocalRotation(rotation);
    // set collider
    m_collider = std::make_unique<AABB>(m_model);
    // configure soket data
    soketConfig();
}

void Player::soketConfig()
{
    m_sokets["RightHand"] = 88;
}

#pragma endregion

#pragma region game_object_update
void Player::Update(float dt)
{
    // update animator
    UpdateAnimation(dt);
    updateWeaponTransform();
    // update height based on terrain
    updateHeight(dt);
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
    glm::vec3 localPos = transform.GetLocalPosition();
    localPos.y = m_worldHeight;
    transform.SetLocalPosition(localPos);
}

#pragma endregion

#pragma region render
void Player::Render(const Frustum &frustum)
{
    // inside frsutum
    if (m_collider->isOnFrustum(frustum, transform))
    {
        m_renderer.Draw(m_shader, transform, m_model, m_animator3D);
    }

    // draw children
    for (auto&& child : children)
    {
        if (auto* renderable = dynamic_cast<Renderable*>(child.get()))
            renderable->Render(frustum);
    }
}

void Player::RenderShadow(const Frustum &frustum)
{
    // inside frsutum
    if (m_collider->isOnFrustum(frustum, transform))
    {
        m_renderer.DrawShadow(transform, m_model, m_animator3D);
    }

    // draw children
    for (auto&& child : children)
    {
        if (auto* renderable = dynamic_cast<Renderable*>(child.get()))
            renderable->RenderShadow(frustum);
    }
}

#pragma endregion

#pragma region player_controls
void Player::Move(Camera_Movement direction, Camera &camera, World &world, float dt)
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
    if (transform.IsDirty()) transform.ComputeModelMatrix();

    glm::vec3 worldPostion = transform.GetGlobalPosition();
    glm::vec3 groundNormal = world.GetWorldNormal(worldPostion.x, worldPostion.z);
    glm::vec3 projectedMove = moveDir - groundNormal * glm::dot(moveDir, groundNormal);
    projectedMove = glm::normalize(projectedMove);

    // set camera distance
    float dot = glm::dot(projectedMove, frontXZ);
    if(dot < 0.0f) camera.targetDistance = BACK_DISTANCE;
    else camera.targetDistance = NORMAL_DISTANCE;

    // result of atan2 is radians, convert radinas to degrees
    glm::vec3 localRot = transform.GetLocalRotation();

    float targetYaw = glm::degrees(atan2(projectedMove.x, projectedMove.z));
    float delta = NormalizeAngle(targetYaw - localRot.y);

    localRot.y += delta * dt * 10.0f;
    transform.SetLocalRotation(localRot);

    // move
    glm::vec3 localPosition = transform.GetLocalPosition();
    localPosition += projectedMove * velocity;
    // set y position
    m_worldHeight = world.GetWorldHeight(transform.GetGlobalPosition().x, transform.GetGlobalPosition().z);
    localPosition.y = m_worldHeight;

    transform.SetLocalPosition(localPosition);
}

float NormalizeAngle(float angle)
{
    while(angle > 180.0f) angle -= 360.0f;
    while(angle < -180.0f) angle += 360.0f;
    return angle;
}

#pragma endregion

#pragma region weapon_control
void Player::AttachWeapon(Weapon *weapon)
{
    m_weapon = weapon;
}

void Player::updateWeaponTransform()
{
    const int soket = m_sokets["RightHand"];
    const auto& bones = m_animator3D.GetGlobalBoneMatrices();

    // bone world matrix
    glm::mat4 boneModel = transform.GetModelMatrix() * bones[soket];
    // weapon
    glm::mat4 weaponLocalMat = m_weapon->transform.GetLocalMatrix();
    glm::mat4 weaponWorldMat = boneModel * weaponLocalMat;

    m_weapon->transform.SetModelMatrix(weaponWorldMat);
}

#pragma endregion