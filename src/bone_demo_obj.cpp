#include "bone_demo_obj.h"
#include "resource_manager.h"

BoneDemoObj::BoneDemoObj()
    : GameObject(), Renderable(), m_renderer(), m_shader(nullptr), m_model(nullptr), Speed(5.0f)
{
}

BoneDemoObj::BoneDemoObj(Model &model, Shader &shader, glm::vec3 position, glm::vec3 size, glm::vec3 rotation, glm::vec2 velocity) : Speed(5.0f)
{
    ObjectTransform.position = position;
    ObjectTransform.scale = size;
    ObjectTransform.rotation = rotation;
    m_model = &model;
    m_shader = &shader;
}

void BoneDemoObj::Update(float dt)
{
    updateHeight(dt);
}

void BoneDemoObj::Render()
{
    m_renderer.Draw(*m_shader, ObjectTransform, *m_model);
}

void BoneDemoObj::RenderShadow()
{
    m_renderer.DrawShadow(ObjectTransform, *m_model);
}

void BoneDemoObj::updateHeight(float dt)
{
    float yDiff = m_terrainHeight - ObjectTransform.position.y;
    // set object height
    if(yDiff > 0.0f)
    {
        ObjectTransform.position.y = m_terrainHeight;
    }
    else
    {
        ObjectTransform.position.y = glm::mix(
            ObjectTransform.position.y,
            m_terrainHeight,
            10.0f * dt
        );
    }
}