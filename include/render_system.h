#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "scene.h"
#include "ubo_data.h"
#include "geometry_pass.h"
#include "pbr_pass.h"
#include "cascaded_shadow_pass.h"

// ubo state : 1 : camera, 2 : dirlight, 3 : cascaded shadow matrices, 4 : csm data
class RenderSystem
{
public:
    //constructor(s)
    RenderSystem(unsigned int width, unsigned int height);
    ~RenderSystem();

    // renderloop functions
    void Init();
    void BeginFrame(Scene *scene);
    void Render(Scene *scene, float dt);
    void EndFrame();

private:
    unsigned int m_cameraUBO, m_dirLightUBO;
    unsigned int m_width, m_height;
    // skybox VAO, VBO
    unsigned int m_cubeVAO, m_cubeVBO;
    // render pass objects
    GeometryPass *m_geometryPass = nullptr;
    PBRPass *m_pbrPass = nullptr;
    CascadedShadowPass *m_cascadedShadowPass = nullptr;

    // ubo
    void configureUBO();
    void updateUBO(Scene *scene);
    // skybox functions
    void renderSkyBox(unsigned int envCubeMap, unsigned int gBuffer);
    void renderCube();
};