#include "render/renderer/skeletal_mesh_renderer.h"
#include "resource_manager.h"
#include "model/model.h"
#include "animation/animator.h"
#include "object/transform.h"

SkeletalMeshRenderer::SkeletalMeshRenderer()
{
    // load shadow shader
    ResourceManager::LoadShader("shaders/csm_shader/boneShadow.vert", "shaders/csm_shader/cascadedShadow.frag", "shaders/csm_shader/cascadedShadow.geom", nullptr, nullptr, "boneCSMShader");
}

void SkeletalMeshRenderer::Draw(Shader &shader, Transform &transform, Model &model, Animator &animator)
{
    // create transformation matrix
    glm::mat4 modelMatrix = transform.GetModelMatrix();

    // set the model matrix uniform
    shader.Use();
    shader.SetMatrix4("model", modelMatrix);
    // set normal amtrix uniform
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    shader.SetMatrix4("normalMatrix", normalMatrix);
    
    auto transforms = animator.GetFinalBoneMatrices();
    for (unsigned int i = 0; i < transforms.size(); ++i){
        shader.SetMatrix4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);
    }

    // draw the model
    for(unsigned int i = 0; i < model.meshes.size(); i++)
    {
        model.meshes[i].Draw(shader);
    }
}

void SkeletalMeshRenderer::DrawShadow(Transform &transform, Model &model, Animator &animator)
{
    // create transformation matrix
    glm::mat4 modelMatrix = transform.GetModelMatrix();

    // set the model matrix uniform
    Shader &shadowShader = ResourceManager::GetShader("boneCSMShader");
    shadowShader.Use();
    shadowShader.SetMatrix4("model", modelMatrix);
    
    auto transforms = animator.GetFinalBoneMatrices();
    for (unsigned int i = 0; i < transforms.size(); ++i){
        shadowShader.SetMatrix4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);
    }

    // draw the model
    for(unsigned int i = 0; i < model.meshes.size(); i++)
    {
        model.meshes[i].Draw(shadowShader);
    }
}