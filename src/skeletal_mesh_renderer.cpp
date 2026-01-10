#include "skeletal_mesh_renderer.h"

void SkeletalMeshRenderer::Draw(Shader &shader, GameObject &gameObject, float deltaTime)
{
    // create transformation matrix
    glm::mat4 modelMatrix = gameObject.ObjectTransform.GetModelMatrix();

    // set the model matrix uniform
    shader.Use();
    shader.SetMatrix4("model", modelMatrix);
    // set normal amtrix uniform
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
    shader.SetMatrix4("normalMatrix", normalMatrix);

    // set animation 
    gameObject.Animator3D.UpdateAnimation(deltaTime);
    
    auto transforms = gameObject.Animator3D.GetFinalBoneMatrices();
    for (unsigned int i = 0; i < transforms.size(); ++i){
        shader.SetMatrix4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);
    }

    // draw the model
    for(unsigned int i = 0; i < gameObject.Model3D->meshes.size(); i++)
    {
        gameObject.Model3D->meshes[i].Draw(shader);
    }
}