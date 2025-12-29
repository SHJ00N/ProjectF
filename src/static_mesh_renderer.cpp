#include "static_mesh_renderer.h"

void StaticMeshRenderer::Draw(Shader &shader, GameObject &gameObject, float deltaTime)
{
    // create transformation matrix
    glm::mat4 modelMatrix = gameObject.ObjectTransform.GetModelMatrix();

    // set the model matrix uniform
    shader.Use();
    shader.SetMatrix4("model", modelMatrix);

    // draw the model
    for(unsigned int i = 0; i < gameObject.Model3D->meshes.size(); i++)
    {
        gameObject.Model3D->meshes[i].Draw(shader);
    }
}