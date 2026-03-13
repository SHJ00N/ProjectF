#include "render/render_system.h"
#include "render/pass/cascaded_shadow_pass.h"
#include "render/pass/geometry_pass.h"
#include "render/pass/pbr_pass.h"
#include "render/pass/screen_space_ambient_occlusion_pass.h"
#include "light/light.h"
#include "light/direction_light.h"
#include "resource_manager.h"
#include "scene/scene.h"
#include "particle/particle.h"
#include "render/pass/sky_box_pass.h"
#include "render/pass/particle_pass.h"
#include "debug/debug_render_pass.h"

#include <iostream>

#pragma region lifecycle
RenderSystem::RenderSystem(unsigned int width, unsigned int height) : m_width(width), m_height(height) 
{
    m_cameraUBO = 0;
    m_dirLightUBO = 0;
}

RenderSystem::~RenderSystem()
{
    if(m_cameraUBO) glDeleteBuffers(1, &m_cameraUBO);
    if(m_dirLightUBO) glDeleteBuffers(1, &m_dirLightUBO);
    delete m_geometryPass;
    delete m_pbrPass;
    delete m_cascadedShadowPass;
    delete m_ssaoPass;
    delete m_skyBoxPass;
    delete m_particlePass;
    delete m_debugPass;
}

#pragma endregion

#pragma region loop
void RenderSystem::Init()
{
    configureUBO();
    // create render pass obj
    m_geometryPass = new GeometryPass(m_width, m_height);
    m_pbrPass = new PBRPass();
    m_cascadedShadowPass = new CascadedShadowPass(m_width, m_height);
    m_ssaoPass = new SSAOPass(m_width, m_height);
    m_skyBoxPass = new SkyBoxPass();
    m_particlePass = new ParticlePass();
    m_debugPass = new DebugPass();
}

void RenderSystem::BeginFrame(Scene *scene)
{
    if(scene->GetRenderType() == RenderType::Deferred)
    {
        updateUBO(scene);
    }
}

void RenderSystem::Render(Scene *scene, float dt)
{
    if(scene->GetRenderType() == RenderType::Deferred)
    {
        // deffered rendering
        // geometry pass
        m_geometryPass->Render(scene);
        // cascaded shadow pass
        m_cascadedShadowPass->Render(scene);
        // screen space ambient occlusion
        m_ssaoPass->Render(m_geometryPass->GetTextures());
        // pbr lighting pass
        m_pbrPass->Render(m_geometryPass->GetTextures(), m_cascadedShadowPass->GetShadowMapTexture(), m_ssaoPass->GetSSAOTexture(), scene->GetIBLData());
        
        // forward rendering
         m_debugPass->RenderAABB(scene, m_geometryPass->GetTextures().depth);
        // render skybox with forward rendering
        m_skyBoxPass->Render(scene->GetIBLData().envCubeMap);
        // particle render
        m_particlePass->Render(scene, m_geometryPass->GetTextures().depth, m_width, m_height);
    }
}

void RenderSystem::EndFrame()
{

}

#pragma endregion

#pragma region UBO
void RenderSystem::configureUBO()
{
    // configure uniform buffer object
    // camera uniform block
    glGenBuffers(1, &m_cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUBO), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // lights uniform block
    glGenBuffers(1, &m_dirLightUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_dirLightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(DirLightBlockUBO), nullptr, GL_STATIC_DRAW);  // dirlight variables don't change frequently
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_dirLightUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RenderSystem::updateUBO(Scene* scene)
{
    // null check
    if(!scene) return;

    // update camera UBO
    Camera* camera = scene->GetCamera();
    if(camera && m_cameraUBO != 0)
    {
        CameraUBO data;
        data.projection = camera->GetProjectionMatrix(m_width, m_height);
        data.view = camera->GetViewMatrix();
        data.cameraPos = camera->cameraPos;
        data.cameraFov = glm::radians(camera->fov);

        glBindBuffer(GL_UNIFORM_BUFFER, m_cameraUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUBO), &data);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // update dirlight UBO
    const auto& lights = scene->GetLights();
    for(const auto& light : lights)
    {
        // set light variables
        if(light->GetLightType() == LightType::Direction && m_dirLightUBO != 0)
        {
            DirLightBlockUBO data;
            // lighting variables
            data.light.colorIntensity.x = light->Color.x;
            data.light.colorIntensity.y = light->Color.y;
            data.light.colorIntensity.z = light->Color.z;
            data.light.colorIntensity.w = light->Intensity;
            data.light.direction = static_cast<DirLight*>(light)->Direction;
            // bool variable
            data.hasDirLight = 1;

            // set light direction for cascaded shadow pass
            m_cascadedShadowPass->SetLightDirection(glm::normalize(data.light.direction));

            // configure uniform buffer data
            glBindBuffer(GL_UNIFORM_BUFFER, m_dirLightUBO);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DirLightBlockUBO), &data);
        }
    }
}

#pragma endregion
