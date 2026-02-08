#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

#include "object/game_object.h"
#include "camera.h"
#include "object/interface/renderable.h"
#include "shader.h"
#include "render/renderer/skeletal_mesh_renderer.h"
#include "object/interface/animatable.h"
#include "animation/animator.h"
#include "object/collider/aabb.h"

class World;
class Model;

const float PLAYER_SPEED = 100.0f;

class Player : public GameObject, public Renderable, public Animatable
{
public:
    float Speed;
    // constructor(s)
    Player(Model &model, Shader &shader, glm::vec3 position, glm::vec3 size, glm::vec3 rotation = glm::vec3(0.0f), glm::vec2 velocity = glm::vec2(0.0f));

    // override functions
    void Update(float dt) override;
    // renderable override
    void Render(const struct Frustum &frustum) override;
    void RenderShadow() override;
    // animatable override
    void UpdateAnimation(float dt) override;
    void SetAnimation(Animation* animation) override;

    // setters
    void SetWorldHeight(float height) { m_worldHeight = height; }

    // player controls
    void Move(Camera_Movement direction, Camera &camera, World &world, float dt);

private:
    // render member
    SkeletalMeshRenderer m_renderer;
    Animator m_animator3D;
    Shader &m_shader;
    Model &m_model;

    // aabb collider
    std::unique_ptr<AABB> m_collider;

    float m_worldHeight = 0.0f;
    // set player y position based on terrain height
    void updateHeight(float dt);
};