#include "object/behavior_tree/behavior/Bcheck_origin_position.h"
#include "object/game_object.h"
#include "object/interface/animatable.h"

CheckOriginPosition::CheckOriginPosition(GameObject &gameObject, glm::vec3 originPos, glm::vec3 originRot, float speed) : m_object(gameObject), m_originPos(originPos), m_originRot(originRot), m_speed(speed)
{
    m_animatable = dynamic_cast<Animatable*>(&m_object);
}

BTState CheckOriginPosition::Evaluate(float dt)
{
    glm::vec3 currentPos = m_object.transform.GetLocalPosition();
    currentPos.y = 0.0f;

    if(glm::distance(currentPos, m_originPos) < 0.01f)
    {
        m_object.transform.SetLocalPosition(m_originPos);
        m_object.transform.SetLocalRotation(m_originRot);
        if(m_animatable) m_animatable->Animator3D.PlayAnimation("Idle");
    }
    else
    {
        // move to waypoint
        if(m_animatable) m_animatable->Animator3D.PlayAnimation("Walk");
        Move(dt);
    }

    state = BTState::Running;
    return state;
}

void CheckOriginPosition::Move(float dt)
{
    glm::vec3 currentPos = m_object.transform.GetLocalPosition();

    glm::vec3 dir = m_originPos - currentPos;
    dir.y = 0.0f;

    float dist = glm::length(dir);

    // prevent overshoot
    if(dist < m_speed * dt)
    {
        m_object.transform.SetLocalPosition(m_originPos);
        m_object.transform.SetLocalRotation(m_originRot);
        return;
    }

    dir = glm::normalize(dir);

    // rotation
    glm::vec3 localRot = m_object.transform.GetLocalRotation();

    float targetYaw = glm::degrees(atan2(dir.x, dir.z));
    float delta = NormalizeAngle(targetYaw - localRot.y);

    if(fabs(delta) > 140.0f)
        localRot.y = targetYaw;
    else
        localRot.y += delta * dt * 6.0f;

    localRot.y = NormalizeAngle(localRot.y);
    m_object.transform.SetLocalRotation(localRot);

    // position
    glm::vec3 newPos = currentPos + dir * m_speed * dt;
    m_object.transform.SetLocalPosition(newPos);
}

float CheckOriginPosition::NormalizeAngle(float angle)
{
    while(angle > 180.0f) angle -= 360.0f;
    while(angle < -180.0f) angle += 360.0f;
    return angle;
}