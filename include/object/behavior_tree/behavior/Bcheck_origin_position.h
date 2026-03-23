#pragma once

#include "object/behavior_tree/BTNode.h"
#include <glm/glm.hpp>

class GameObject;
class Animatable;

class CheckOriginPosition : public BTNode
{
public:
    CheckOriginPosition(GameObject &gameObject, glm::vec3 originPos, glm::vec3 originRot, float speed);
    // override
    BTState Evaluate(float dt) override;

private:
    GameObject &m_object;
    Animatable *m_animatable = nullptr;

    glm::vec3 m_originPos;
    glm::vec3 m_originRot;

    float m_speed;

    // object move to waypoint
    void Move(float dt);
    float NormalizeAngle(float angle);
};