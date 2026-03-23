#include "object/enemy/enemy.h"
#include "world/world.h"
#include "frustum.h"
#include "particle/particle_manager.h"
#include "object/weapon.h"

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

void Enemy::EquipWeapon(Model &model, Shader &shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    m_weapon = &this->addChild<Weapon>(model, shader, position, rotation, scale, Layer::Weapon);
    m_weapon->SetOwner(this);
}

Weapon* Enemy::GetWeapon() const
{
    return m_weapon;
}

#pragma region override
void Enemy::Init()
{
    Health = 90;
    IsHit = false;
    IsDeath = false;
    IsAttacking = false;

    m_deathTimer = 0.0f;
}

void Enemy::Update(const ObjectUpdateContext &context)
{
    float dt = context.deltaTime;
    UpdateAnimation(dt);
    
    if(m_BT) m_BT->Update(context);
    
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
    if(IsDeath)
    {
        m_deathTimer += dt;
        if(m_deathTimer >= 5.0f)
        {
            ObjectDestroyed = true;
            EntityDestroyed = true;
            RenderableDestroyed = true;
            CollidableDestroyed = true;
        }
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
    if(IsHit || IsDeath) return;

    IsHit = true;
    SetAnimation(Animator3D.GetAnimation("Hit"), true);
    Health -= damage;
    ParticleManager::SpawnBloodParticle(this, GetSocketLocalPosition("Center") + glm::vec3(0.0f, 40.0f, 0.0f));

    if(Health <= 0) 
    {
        IsDeath = true;
        SetAnimation(Animator3D.GetAnimation("Death"), true);
    }
}

void Enemy::SocketConfig()
{
    m_sockets["RightHand"] = 85;
    m_sockets["Center"] = 10;
}
#pragma endregion

#pragma region private
void Enemy::updateWorldHeight(const World &world)
{
    glm::vec3 localPos = transform.GetLocalPosition();
    float height = world.GetWorldHeight(localPos.x, localPos.z);
    if(localPos.y == height) return;

    localPos.y = height;
    transform.SetLocalPosition(localPos);
}
#pragma endregion

void Enemy::SetupBehaviorTree(std::vector<glm::vec3> &wayPoints, GameObject &target)
{
    m_BT = std::make_unique<EnemyBT>(*this, wayPoints, target);
    m_BT->Init();
}