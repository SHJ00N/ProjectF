#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "model.h"
#include "camera.h"
#include "chunk.h"
#include "renderable.h"
#include "shader.h"
#include "skeletal_mesh_renderer.h"
#include "animatable.h"

const float PLAYER_SPEED = 10.0f;

class Player : public GameObject, public Renderable, public Animatable
{
public:
    float Speed;
    // constructor(s)
    Player();
    Player(Model &model, Shader &shader, glm::vec3 position, glm::vec3 size, glm::vec3 rotation = glm::vec3(0.0f), glm::vec2 velocity = glm::vec2(0.0f));

    // override functions
    void Update(float dt) override;
    // renderable override
    void Render() override;
    void RenderShadow() override;
    // animatable override
    void UpdateAnimation(float dt) override;
    void SetAnimation(Animation* animation) override;

    // setters
    void SetTerrainHeight(float height) { m_terrainHeight = height; }

    // player controls
    void Move(Camera_Movement direction, Camera &camera, Chunk *chunk, float dt);

private:
    // render member
    SkeletalMeshRenderer m_renderer;
    Animator m_animator3D;
    Shader *m_shader;
    Model *m_model;

    float m_terrainHeight = 0.0f;
    // set player y position based on terrain height
    void updateHeight(float dt);
};