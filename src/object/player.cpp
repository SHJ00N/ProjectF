#include "object/player.h"
#include "world/world.h"
#include "frustum.h"
#include "object/weapon.h"

#include <iostream>

float NormalizeAngle(float angle);

Player::Player(Model &model, Shader &shader, glm::vec3 position, glm::vec3 size, glm::vec3 rotation, glm::vec2 velocity, Layer layer) : Speed(PLAYER_SPEED), m_model(model), m_shader(shader)
{
    // set layout
    ObjectLayer = layer;
    // set transform info
    transform.SetLocalPosition(position);
    transform.SetLocalScale(size);
    transform.SetLocalRotation(rotation);
    updateSelfAndChild();
    // create collider
    m_aabb = std::make_unique<AABB>(m_model);
    m_aabb->Owner = this;
    // configure soket data
    InitSocket(&Animator3D, &transform);
}

void Player::EquipWeapon(Weapon* weapon)
{
    m_weapon = weapon;
}

#pragma region override_functions
void Player::Update(const ObjectUpdateContext &context)
{
    float dt = context.deltaTime;
    
    UpdateAnimation(dt);
    
    // update action
    updateAction(dt);
    // update motion and move 
    updateMotion();
    move(*context.camera, dt);
    updateAnimation(); // update animation based on motion and action state
    
    updateWorldHeight(*context.world);  // update height based on world height

    if(transform.IsDirty())
    {
        updateSelfAndChild();
        Transform colTransform = transform;
        colTransform.SetLocalRotation(transform.GetLocalRotation() + axisFix);
        colTransform.ComputeModelMatrix();
        m_aabb->UpdateGlobalAABB(collider.worldAABB, colTransform);
        context.world->UpdateChunkCollider(collider);
    }

    if(ObjectDestroyed) 
    {
        EntityDestroyed = true;
        RenderableDestroyed = true;
        CollidableDestroyed = true;
    }

    for(auto&& child : children)
    {
        auto* gameObject = dynamic_cast<GameObject*>(child.get());
        if(gameObject) gameObject->Update(context);
    }
}

void Player::Render(const Frustum &frustum)
{
    // inside frsutum
    if (m_aabb->isOnFrustum(frustum, transform))
    {
        m_renderer.Draw(m_shader, transform, m_model, Animator3D);
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
    if (m_aabb->isOnFrustum(frustum, transform))
    {
        m_renderer.DrawShadow(transform, m_model, Animator3D);
    }

    // draw children
    for (auto&& child : children)
    {
        if (auto* renderable = dynamic_cast<Renderable*>(child.get()))
            renderable->RenderShadow(frustum);
    }
}

void Player::SocketConfig()
{
    m_sockets["RightHand"] = 88;
    m_sockets["Center"] = 27;
}
#pragma endregion

#pragma region player_controls
// public
// ----------------------------------------------------------------------
void Player::RequestMove(const glm::vec3 direction, bool isRunning)
{
    m_inputMoveDirection = direction;
    m_inputIsRunning = isRunning;
    
    if(m_actionState == ActionState::Attack1) return; // if performing attack, ignore move input

    m_moveDirection = direction;
    m_isRunning = isRunning;
}

void Player::RequestAttack()
{
    // if already acting, store input to action input buffer
    if(m_actionState != ActionState::None) {
        if(canCancelAction())
        {
            m_weapon->EndAttack();
            m_inputActionState = ActionState::Attack1;
            m_actionInputBufferTime = ACTION_INPUT_BUFFER_TIME; // reset input buffer timer
        }
        return; 
    }

    startAction(ActionState::Attack1);
}

void Player::RequestRoll()
{
    // if already acting, store input to action input buffer
    if(m_actionState != ActionState::None) {
        if(canCancelAction())
        {
            m_inputActionState = ActionState::Roll; // store input action state to perform after current action finishes
            m_actionInputBufferTime = ACTION_INPUT_BUFFER_TIME;
        }
        return; 
    }

    startAction(ActionState::Roll);
}

void Player::RequestHit()
{
    m_inputActionState = ActionState::Hit;
    m_actionInputBufferTime = ACTION_INPUT_BUFFER_TIME;
    startAction(ActionState::Hit);
}

ActionState Player::GetActionState() const
{
    return m_actionState;
}

MotionState Player::GetMotionState() const
{
    return m_motionState;
}

// private
// ----------------------------------------------------------------------
void Player::updateWorldHeight(const World &world)
{
    glm::vec3 localPos = transform.GetLocalPosition();
    float height = world.GetWorldHeight(localPos.x, localPos.z);
    if(localPos.y == height) return;

    localPos.y = height;
    transform.SetLocalPosition(localPos);
}

void Player::startAction(ActionState action)
{
    Animator3D.GetRootMotionDelta(); // reset root motion delta to prevent sudden position change when starting action

    m_actionState = action;
    m_actionTimer = 0.0f;
    m_moveDirection = glm::vec3(0.0f);
    m_isRunning = false;

    // if animation playing is forced, animation will play without blending from current animation
    if(action == ActionState::Attack1)
    {
        SetAnimation(Animator3D.GetAnimation("Attack1"), true); // force play attack animation
    }
    else if(action == ActionState::Roll)
    {
        SetAnimation(Animator3D.GetAnimation("Roll"), true); // force play roll animation
    }
    else if(action == ActionState::Hit)
    {
        SetAnimation(Animator3D.GetAnimation("Hit"), true); // force play hit animation
    }
}

void Player::updateAction(float dt)
{
    if(m_actionState == ActionState::None) return;

    // if there is buffered action input and can cancel current action, start buffered action
    if(m_inputActionState != ActionState::None && canCancelAction())
    {
        startAction(m_inputActionState);
        m_inputActionState = ActionState::None;
        return;
    }

    m_actionTimer += dt;
    action(dt);

    m_actionInputBufferTime -= dt;
    if(m_actionInputBufferTime <= 0.0f)
    {
        m_inputActionState = ActionState::None; // clear buffered input if time window has passed
    }
}

void Player::transitionFromAction()
{
    // if attack animation finished, check if there is buffered action input to perform next
    m_actionState = m_inputActionState;
    m_inputActionState = ActionState::None;
    m_actionInputBufferTime = 0.0f;

    // restore input movement
    m_moveDirection = m_inputMoveDirection;
    m_isRunning = m_inputIsRunning;

    if(m_actionState != ActionState::None) startAction(m_actionState); // start next action if there is buffered input
}

bool Player::canCancelAction() const
{
    if(m_actionState == ActionState::None) return true;
    auto it = m_actionCancelTable.find(m_actionState);
    if(it == m_actionCancelTable.end()) return false; // if action state not found in cancel table, cannot cancel
    return m_actionTimer >= it->second; // can cancel if action timer has passed cancel time for current action state
}

void Player::transformFromActionAnimation(float dt)
{
    // apply position delta from root motion
    glm::vec3 rootMotionDelta = Animator3D.GetRootMotionDelta();
    glm::vec3 localPosition = transform.GetLocalPosition();
    glm::vec3 localRotation = transform.GetLocalRotation();

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(localRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

    localPosition += glm::vec3(rotationMatrix * glm::vec4(rootMotionDelta, 0.0f)) * dt * 2.0f; // scale root motion delta for better movement feel
    transform.SetLocalPosition(localPosition);
}

void Player::updateMotion()
{
    if(m_actionState != ActionState::None) return;

    if(glm::length2(m_moveDirection) < 0.0001f)
    {
        m_motionState = MotionState::Idle;
        return;
    }

    m_motionState = m_isRunning ? MotionState::Run : MotionState::Walk;
}

void Player::updateAnimation()
{
    // if performing action, don't update animation based on motion state
    if(m_actionState != ActionState::None) return; 

    // update animation based on motion state
    if(m_motionState == MotionState::Idle)
    {
        SetAnimation(Animator3D.GetAnimation("Idle"));
    }
    else if(m_motionState == MotionState::Walk)
    {
        SetAnimation(Animator3D.GetAnimation("Walk"));
    }
    else if(m_motionState == MotionState::Run)
    {
        SetAnimation(Animator3D.GetAnimation("Run"));
    }
}

void Player::move(Camera &camera, float dt)
{
    if(glm::length2(m_moveDirection) < 0.0001f) return;
    if(m_actionState == ActionState::Attack1 || m_actionState == ActionState::Hit) return;

    float velocity = Speed * dt * (m_isRunning ? 2.5f : 1.0f); // increase speed if running
    if(m_actionState == ActionState::Roll) velocity = 0.0f; // if rolling, only change direction

    // move direction
    glm::vec3 frontXZ = glm::normalize(glm::vec3(camera.cameraFront.x, 0.0f, camera.cameraFront.z));
    glm::vec3 rightXZ = glm::normalize(glm::cross(frontXZ, camera.worldUp));

    glm::vec3 moveDir = frontXZ * m_moveDirection.z + rightXZ * m_moveDirection.x; 
    moveDir = glm::normalize(moveDir);

    // set camera distance
    camera.targetDistance = NORMAL_DISTANCE;

    // rotate to move direction
    glm::vec3 localRot = transform.GetLocalRotation();
    float targetYaw = glm::degrees(atan2(moveDir.x, moveDir.z));
    // calculate angle difference
    float delta = NormalizeAngle(targetYaw - localRot.y);

    if (fabs(delta) > 140.0f)
        localRot.y = targetYaw;
    else
        localRot.y += delta * dt * 6.0f;
    localRot.y = NormalizeAngle(localRot.y);
    transform.SetLocalRotation(localRot);

    // move
    glm::vec3 localPosition = transform.GetLocalPosition();
    localPosition += moveDir * velocity;
    transform.SetLocalPosition(localPosition);
}

float NormalizeAngle(float angle)
{
    while(angle > 180.0f) angle -= 360.0f;
    while(angle < -180.0f) angle += 360.0f;
    return angle;
}

void Player::action(float dt)
{
    if(Animator3D.IsAnimationFinished())
    {
        m_weapon->EndAttack();
        transitionFromAction();
        return;
    }

    if(m_actionState == ActionState::Attack1)
    {
        if(m_actionTimer >= 1.0f)
        {
            m_weapon->EndAttack();
        }
        else if(m_actionTimer >= 0.5f)
        {
            if(!m_weapon->IsAttacking()) m_weapon->StartAttack();
        }
    }

    transformFromActionAnimation(dt); // apply root motion from animation
}

#pragma endregion