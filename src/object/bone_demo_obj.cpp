#include "object/bone_demo_obj.h"
#include "resource_manager.h"

BoneDemoObj::BoneDemoObj(Model &model, Shader &shader, glm::vec3 position, glm::vec3 size, glm::vec3 rotation, glm::vec2 velocity) : Speed(5.0f), m_model(model), m_shader(shader)
{
    transform.SetLocalPosition(position);
    transform.SetLocalScale(size);
    transform.SetLocalRotation(rotation);

    m_collider = std::make_unique<AABB>(m_model);
}

void BoneDemoObj::Update(float dt)
{
    updateHeight(dt);

    if (transform.IsDirty()) transform.ComputeModelMatrix();
}

void BoneDemoObj::Render(const struct Frustum& frustum)
{
    if (m_collider->isOnFrustum(frustum, transform))
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