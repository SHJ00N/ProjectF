#include "render/renderer/static_mesh_renderer.h"
#include "resource_manager.h"
#include "model/model.h"

StaticMeshRenderer::StaticMeshRenderer()
{
    ResourceManager::LoadShader("shaders/csm_shader/staticMeshShadow.vert", "shaders/csm_shader/cascadedShadow.frag", "shaders/csm_shader/cascadedShadow.geom", nullptr, nullptr,"staticMeshCSMShader");
}

void StaticMeshRenderer::Draw(Shader &shader, Transform &transform, Model &model)
{
    // create transformation matrix
    glm::mat4 modelMatrix = transform.GetModelMatrix();

    // set the model matrix uniform
    shader.Use();
    shader.SetMatrix4("model", modelMatrix);

    // draw the model
    for(unsigned int i = 0; i < model.meshes.size(); i++)
    {
        model.meshes[i].Draw(shader);
    }
}

void StaticMeshRenderer::DrawShadow(Transform &transform, Model &model)
{
    Shader &shadowShader = ResourceManager::GetShader("staticMeshCSMShader");
    // create transformation matrix
    glm::mat4 modelMatrix = transform.GetModelMatrix();
    // set the model matrix uniform
    shadowShader.Use();
    shadowShader.SetMatrix4("model", modelMatrix);
    // draw the model
    for(unsigned int i = 0; i < model.meshes.size(); i++)
    {
        model.meshes[i].Draw(shadowShader);
    }
}