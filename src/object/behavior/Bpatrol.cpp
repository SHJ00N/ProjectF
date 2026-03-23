#include "object/behavior_tree/behavior/Bpatrol.h"
#include "object/game_object.h"
#include "object/interface/animatable.h"

Patrol::Patrol(GameObject &gameObject, std::vector<glm::vec3> &wayPoints, float speed) : m_object(gameObject), m_wayPoints(wayPoints), m_speed(speed)
{
    m_animatable = dynamic_cast<Animatable*>(&m_object);
}

BTState Patrol::Evaluate(float dt)
{
    if(m_waiting)
    {
        m_waitTimer += dt;
        // waiting end and start patrol
        if(m_waitTimer >= m_waitTime)
        {
            m_waiting = false;
            if(m_animatable) m_animatable->Animator3D.PlayAnimation("Walk");
        }
    }
    else
    {
        glm::vec3 currentPos = m_object.transform.GetLocalPosition();
        currentPos.y = 0.0f;
        glm::vec3 wp = m_wayPoints[m_currentWayPointIndex];
        // patrol end and start wait
        if(glm::distance(currentPos, wp) < 0.01f)
        {
            m_object.transform.SetLocalPosition(wp);
            m_waitTimer = 0.0f;
            m_waiting = true;

            m_currentWayPointIndex = (m_currentWayPointIndex + 1) % m_wayPoints.size();
            if(m_animatable) m_animatable->Animator3D.PlayAnimation("Idle");
        }
        else
        {
            // move to waypoint
            if(m_animatable) m_animatable->Animator3D.PlayAnimation("Walk");
            Move(wp, dt);
        }
    }

    state = BTState::Running;
    return state;
}

void Patrol::Move(const glm::vec3 &wayPoint, float dt)
{
    glm::vec3 currentPos = m_object.transform.GetLocalPosition();

    glm::vec3 dir = wayPoint - currentPos;
    dir.y = 0.0f;

    float dist = glm::length(dir);

    // prevent overshoot
    if(dist < m_speed * dt)
    {
        m_object.transform.SetLocalPosition(wayPoint);
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

float Patrol::NormalizeAngle(float angle)
{
    while(angle > 180.0f) angle -= 360.0f;
    while(angle < -180.0f) angle += 360.0f;
    return angle;
}