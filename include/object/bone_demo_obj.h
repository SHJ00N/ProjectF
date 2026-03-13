#pragma once

#include "object/game_object.h"
#include "object/interface/renderable.h"
#include "model/model.h"
#include "render/renderer/static_mesh_renderer.h"
#include "shader.h"
#include "camera.h"
#include "object/collider/aabb.h"
#include "object/collider/box_collider.h"
#include "object/interface/collidable.h"
#include "object/interface/damageable.h"

#include <memory>

class BoneDemoObj : public GameObject, public Renderable, public Collidable, public Damageable
{
public:
    float Speed;
    int Health = 10000;
    // constructor(s)
    BoneDemoObj(Model &model, Shader &shader, glm::vec3 position, glm::vec3 size, glm::vec3 rotation = glm::vec3(0.0f), glm::vec2 velocity = glm::vec2(0.0f), Layer layer = Layer::None);

    // override functions
    void Update(const ObjectUpdateContext &context) override;
    // renderable override
    void Render(const struct Frustum &frustum) override;
    void RenderShadow(const struct Frustum& frustum) override;

    void TakeDamage(int damage) override;

    // setters
    void SetWorldHeight(float height) { m_worldHeight = height; }

private:
    // render member
    StaticMeshRenderer m_renderer;
    Shader &m_shader;
    Model &m_model;

    std::unique_ptr<AABB> m_aabb;

    float m_worldHeight = 0.0f;
    // set player y position based on terrain height
    void updateHeight(float dt);
};