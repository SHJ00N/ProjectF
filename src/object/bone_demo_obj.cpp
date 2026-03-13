#include "object/bone_demo_obj.h"
#include "resource_manager.h"
#include "world/world.h"
#include "particle/particle_manager.h"

#include <iostream>

BoneDemoObj::BoneDemoObj(Model &model, Shader &shader, glm::vec3 position, glm::vec3 size, glm::vec3 rotation, glm::vec2 velocity, Layer layer) : Speed(5.0f), m_model(model), m_shader(shader)
{
    ObjectLayer = layer;
    transform.SetLocalPosition(position);
    transform.SetLocalScale(size);
    transform.SetLocalRotation(rotation);
    transform.ComputeModelMatrix();

    m_aabb = std::make_unique<AABB>(m_model);
    m_aabb->Owner = this;
    collider.worldAABB = m_aabb->GetGlobalAABB(transform);
}

void BoneDemoObj::Update(const ObjectUpdateContext &context)
{
    float dt = context.deltaTime;
    
    updateHeight(dt);

    if (transform.IsDirty())
    {
        transform.ComputeModelMatrix();
        m_aabb->UpdateGlobalAABB(collider.worldAABB, transform);
        context.world->UpdateChunkCollider(collider);
    }

    if(ObjectDestroyed) 
    {
        EntityDestroyed = true;
        RenderableDestroyed = true;
        CollidableDestroyed = true;
    }
}

void BoneDemoObj::Render(const struct Frustum& frustum)
{
    if (m_aabb->isOnFrustum(frustum, transform))
    {
        m_renderer.Draw(m_shader, transform, m_model);
    }

    for (auto&& child : children)
    {
        if (auto* renderable = dynamic_cast<Renderable*>(child.get()))
            renderable->Render(frustum);
    }
}

void BoneDemoObj::RenderShadow(const struct Frustum& frustum)
{
    m_renderer.DrawShadow(transform, m_model);
}

void BoneDemoObj::updateHeight(float dt)
{
    glm::vec3 localPos = transform.GetLocalPosition();
    localPos.y = m_worldHeight;
    transform.SetLocalPosition(localPos);
}

void BoneDemoObj::TakeDamage(int damage)
{
    Health -= damage;
    ParticleManager::SpawnBloodParticle(this, glm::vec3(0.0f));

    std::cout << "take damage" << std::endl;
    if(Health <= 0) 
    {
        std::cout << "destroy" << std::endl;
        ObjectDestroyed = true;
    }
} 