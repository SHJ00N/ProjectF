#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <unordered_set>

#include "object/game_object.h"
#include "object/interface/renderable.h"
#include "render/renderer/static_mesh_renderer.h"
#include "object/collider/aabb.h"
#include "object/interface/collidable.h"

class Model;
class Shader;
class Player;

class Weapon : public GameObject, public Renderable, public Collidable
{
public:
    int Damage = 30;
    // constructor
    Weapon(Model &model, Shader &shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Layer layer = Layer::None);
    // override functions
    void Update(const ObjectUpdateContext &context) final;
    void Render(const struct Frustum& frustum) final;
    void RenderShadow(const struct Frustum& frustum) final;

    void OnCollisionEnter(Collider *other) override;

    void StartAttack();
    void EndAttack();
    bool IsAttacking();

    // setter
    void SetOwner(Player *owner);
private:
    Shader &m_shader;
    Model &m_model;
    StaticMeshRenderer m_renderer;
    Player *m_owner = nullptr; // the player who holds this weapon
    // collider
    std::unique_ptr<AABB> m_aabb;

    // attack member
    std::unordered_set<GameObject*> m_hitTargets;
    bool m_isAttackActive = false;

    // offset
    glm::mat4 m_offsetMatrix;

    void updateTransform();
};