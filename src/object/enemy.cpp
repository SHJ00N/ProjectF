#include "object/enemy.h"
#include "world/world.h"
#include "frustum.h"
#include "particle/particle_manager.h"

#include <iostream>

Enemy::Enemy(Model &model, Shader &shader, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, glm::vec2 velocity, Layer layer) : m_model(model), m_shader(shader)
{
    // set layout
    ObjectLayer = layer;
    // set transform
    transform.SetLocalPosition(position);
    transform.SetLocalScale(scale);
    transform.SetLocalRotation(rotation);
    updateSelfAndChild();
    // set collider
    m_aabb = std::make_unique<AABB>(m_model);
    m_aabb->Owner = this;
    // set socket
    InitSocket(&Animator3D, &transform);
}

#pragma region override
void Enemy::Update(const ObjectUpdateContext &context)
{
    float dt = context.deltaTime;
    SetAnimation(Animator3D.GetAnimation("Idle"), false);
    UpdateAnimation(dt);
    updateWorldHeight(*context.world);

    if(transform.IsDirty())
    {
        updateSelfAndChild();
    }

    // update collider
    Transform colTransform = transform;
    colTransform.SetLocalRotation(transform.GetLocalRotation() + axisFix);
    colTransform.ComputeModelMatrix();
    m_aabb->UpdateGlobalAABB(collider.worldAABB, colTransform);
    context.world->UpdateChunkCollider(collider);
    
    // check destroyed
    if(ObjectDestroyed)
    {
        EntityDestroyed = true;
        RenderableDestroyed = true;
        CollidableDestroyed = true;
    }

    // update child
    for(auto &child : children)
    {
        auto *gameObject = dynamic_cast<GameObject*>(child.get());
        if(gameObject) gameObject->Update(context);
    }
}

void Enemy::Render(const Frustum &frustum)
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
void Enemy::RenderShadow(const Frustum &frustum)
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

void Enemy::TakeDamage(int damage)
{
    std::cout << "hit!" << std::endl;
    Health -= damage;
    ParticleManager::SpawnBloodParticle(this, GetSocketLocalPosition("Center"));

    if(Health <= 0) 
    {
        ObjectDestroyed = true;
    }
}

void Enemy::SocketConfig()
{
    m_sockets["RightHand"] = 88;
    m_sockets["Center"] = 27;
}
#pragma endregion

void Enemy::updateWorldHeight(const World &world)
{
    glm::vec3 localPos = transform.GetLocalPosition();
    float height = world.GetWorldHeight(localPos.x, localPos.z);
    if(localPos.y == height) return;

    localPos.y = height;
    transform.SetLocalPosition(localPos);
}