#include "object/weapon.h"

Weapon::Weapon(Model &model, Shader &shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
: m_shader(shader), m_model(model)
{
    // set transform
    transform.SetLocalPosition(position);
    transform.SetLocalRotation(rotation);
    transform.SetLocalScale(scale);
    // generate collider
    m_collider = std::make_unique<AABB>(m_model);
}

void Weapon::Render(const Frustum &frustum)
{
    // inside frsutum
    if (m_collider->isOnFrustum(frustum, transform))
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
    // inside frsutum
    if (m_collider->isOnFrustum(frustum, transform))
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