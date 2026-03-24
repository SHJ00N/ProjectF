#include "object/weapon.h"
#include "object/player.h"
#include "world/world.h"
#include "object/interface/damageable.h"
#include "sound_manager.h"

#include <iostream>

Weapon::Weapon(Model &model, Shader &shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Layer layer)
: m_shader(shader), m_model(model)
{
    ObjectLayer = layer;
    // set offset Matrix
    glm::mat4 offsetMatrix = glm::mat4(1.0f);
    offsetMatrix = glm::translate(offsetMatrix, position);
    offsetMatrix = glm::rotate(offsetMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    offsetMatrix = glm::rotate(offsetMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    offsetMatrix = glm::rotate(offsetMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    offsetMatrix = glm::scale(offsetMatrix, scale);
    m_offsetMatrix = offsetMatrix;
    // set transform
    updateSelfAndChild();
    // generate collider
    m_aabb = std::make_unique<AABB>(m_model);
    m_aabb->Owner = this;
    collider.worldAABB = m_aabb->GetGlobalAABB(transform);
}

void Weapon::Update(const ObjectUpdateContext &context)
{
    updateTransform();
    m_aabb->UpdateGlobalAABB(collider.worldAABB, transform);
    context.world->UpdateChunkCollider(collider);

    if(ObjectDestroyed) 
    {
        EntityDestroyed = true;
        RenderableDestroyed = true;
        CollidableDestroyed = true;
    }
}

void Weapon::Render(const Frustum &frustum)
{
    if(!RenderActive) return;
    // inside frsutum
    if (m_aabb->isOnFrustum(frustum, transform))
    {
        m_renderer.Draw(m_shader, transform, m_model);
    }

    // draw children
    for (auto&& child : children)
    {
        if (auto* renderable = dynamic_cast<Renderable*>(child.get()))
            renderable->Render(frustum);
    }
}

void Weapon::RenderShadow(const struct Frustum& frustum)
{
    if(!RenderActive) return;
    // inside frsutum
    if (m_aabb->isOnFrustum(frustum, transform))
    {
        m_renderer.DrawShadow(transform, m_model);
    }

    // draw children
    for (auto&& child : children)
    {
        if (auto* renderable = dynamic_cast<Renderable*>(child.get()))
            renderable->RenderShadow(frustum);
    }
}

void Weapon::OnCollisionEnter(Collider *other)
{
    if(!m_isAttackActive) return;

    if(m_owner->ObjectLayer == Layer::Player && other->Owner->ObjectLayer == Layer::Enemy
    || m_owner->ObjectLayer == Layer::Enemy && other->Owner->ObjectLayer == Layer::Player)
    {
        if(m_hitTargets.find(other->Owner) != m_hitTargets.end()) return;
        Damageable *damageable = dynamic_cast<Damageable*>(other->Owner);
        if(!damageable) return;

        damageable->TakeDamage(Damage);

        m_hitTargets.insert(other->Owner);
    }
}

void Weapon::StartAttack()
{
    SoundManager::GetInstance().PlaySFX("slash", 0.8f);
    m_isAttackActive = true;
    m_hitTargets.clear();
}

void Weapon::EndAttack()
{
    m_isAttackActive = false;
}

bool Weapon::IsAttacking()
{
    return m_isAttackActive;
}

void Weapon::SetOwner(GameObject *owner)
{
    m_owner = owner;
    m_socket = dynamic_cast<ISocket*>(owner);
}

void Weapon::updateTransform()
{
    if(!m_socket || !m_owner) return;

    const std::string socket = "RightHand";
    glm::mat4 mat = m_socket->GetSocketGlobalMatrix(socket) * m_offsetMatrix;
    transform.SetModelMatrix(mat);
}