#include "skeletal_mesh_renderer.h"

void SkeletalMeshRenderer::Draw(Shader &shader, GameObject &gameObject, float deltaTime)
{
    // create transformation matrix
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, gameObject.Position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(gameObject.RotationAngle), gameObject.RotationAxis);
    modelMatrix = glm::scale(modelMatrix, gameObject.Size);

    // set the model matrix uniform
    shader.Use();
    shader.SetMatrix4("model", modelMatrix);

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