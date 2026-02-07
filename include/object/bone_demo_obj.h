#pragma once

#include "object/game_object.h"
#include "object/interface/renderable.h"
#include "model/model.h"
#include "render/renderer/static_mesh_renderer.h"
#include "shader.h"
#include "camera.h"

class BoneDemoObj : public GameObject, public Renderable
{
public:
    float Speed;
    // constructor(s)
    BoneDemoObj();
    BoneDemoObj(Model &model, Shader &shader, glm::vec3 position, glm::vec3 size, glm::vec3 rotation = glm::vec3(0.0f), glm::vec2 velocity = glm::vec2(0.0f));

    // override functions
    void Update(float dt) override;
    // renderable override
    void Render() override;
    void RenderShadow() override;

    // setters
    void SetTerrainHeight(float height) { m_terrainHeight = height; }

private:
    // render member
    StaticMeshRenderer m_renderer;
    Shader *m_shader;
    Model *m_model;

    float m_terrainHeight = 0.0f;
    // set player y position based on terrain height
    void updateHeight(float dt);
};