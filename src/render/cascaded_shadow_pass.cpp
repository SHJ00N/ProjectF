#include "render/pass/cascaded_shadow_pass.h"
#include "resource_manager.h"
#include "object/interface/renderable.h"
#include "scene/scene.h"

#include <iostream>
#include <cmath>

std::vector<float> computeCascadeSplits(float nearPlane, float farPlane, size_t cascadeCount);

CascadedShadowPass::CascadedShadowPass(unsigned int width, unsigned int height)
    : m_matricesUBO(0), m_csmDataUBO(0), m_shadowMapFBO(0), m_shadowMap(0), m_lightDir(glm::vec3(0.0f, -1.0f, 0.0f)), m_width(width), m_height(height)
{
    camera = nullptr;

    // create shadow map FBO and texture
    glGenFramebuffers(1, &m_shadowMapFBO);
    // depth texture
    glGenTextures(1, &m_shadowMap);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_shadowMap);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, CASCADE_SIZE + 1, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    // create UBO for light space matrices
    glGenBuffers(1, &m_matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CSMatricesUBO), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // create UBO for CSM data
    glGenBuffers(1, &m_csmDataUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_csmDataUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CSMdataUBO), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 3, m_csmDataUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // create shadow offset texture for PCF
    m_offsetTexture = new ShadowMapOffsetTexture(8, 3, 0.7f);
}

CascadedShadowPass::~CascadedShadowPass()
{
    if (m_matricesUBO) glDeleteBuffers(1, &m_matricesUBO);
    if (m_csmDataUBO) glDeleteBuffers(1, &m_csmDataUBO);
    if (m_shadowMapFBO) glDeleteFramebuffers(1, &m_shadowMapFBO);
    if (m_shadowMap) glDeleteTextures(1, &m_shadowMap);
    
    delete m_offsetTexture;
}

void CascadedShadowPass::Configure(Scene* scene)
{
    // get main camera
    camera = scene->GetCamera();

    // define cascade levels (tune these values as needed)
    float farPlane = camera->farPlane;
    float nearPlane = camera->nearPlane;
    m_shadowCascadeLevels.clear();
    m_shadowCascadeLevels = computeCascadeSplits(nearPlane, farPlane, CASCADE_SIZE);
    // update light space matrices UBO
    const auto lightMatrices = getLightSpaceMatrices();
    glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO); 
    for(size_t i = 0; i < lightMatrices.size(); ++i) 
    { 
        glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]); 
    }
    // update CSM data UBO
    glBindBuffer(GL_UNIFORM_BUFFER, m_csmDataUBO);
    size_t offset = 0;
    // cascadePlaneDistances (std140 stride = 16)
    for (size_t i = 0; i < m_shadowCascadeLevels.size(); ++i)
    {
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &m_shadowCascadeLevels[i]);
        offset += 16;
    }
    // cascadeCount (std140 slot)
    int count = static_cast<int>(m_shadowCascadeLevels.size());
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(int), &count);
    offset += 16;

    // bind and configure framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMapFBO);
    glViewport(0, 0, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
    glClear(GL_DEPTH_BUFFER_BIT);
    // opengl state
    glCullFace(GL_FRONT);
    //glEnable(GL_POLYGON_OFFSET_FILL);
    //glPolygonOffset(2.0f, 1.0f);
}

void CascadedShadowPass::Render(Scene* scene)
{
    // bind and configure framebuffer
    Configure(scene);
    // render world
    //if(scene->GetTerrainRenderer()) scene->GetTerrainRenderer()->RenderShadow(); 
    // render scene from light's point of view
    for(const auto &renderableObj : scene->GetRenderables())
    {
        renderableObj->RenderShadow();
    }

    // reset opengl state
    glCullFace(GL_BACK);
    //glDisable(GL_POLYGON_OFFSET_FILL);
    // unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_width, m_height);
}

std::vector<glm::vec4> CascadedShadowPass::getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4 &view)
{
    const auto inv = glm::inverse(proj * view);
    
    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = 
                    inv * glm::vec4(
                        2.0f * x - 1.0f,
                        2.0f * y - 1.0f,
                        2.0f * z - 1.0f,
                        1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }
    
    return frustumCorners;
}

glm::mat4 CascadedShadowPass::getLightSpaceMatrix(const float nearPlane, const float farPlane)
{
    const glm::mat4 proj = glm::perspective(glm::radians(camera->fov), (float)m_width / (float)m_height, nearPlane, farPlane);
    const auto corners = getFrustumCornersWorldSpace(proj, camera->GetViewMatrix());

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const auto lightView = glm::lookAt(center - m_lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto& v : corners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    auto temp = -minZ;
    minZ = -maxZ;
    maxZ = temp;

    auto mid = (maxZ - minZ) / 2;
    minZ -= mid * 10.0f;
    maxZ += mid * 10.0f;

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection * lightView;
}

std::vector<glm::mat4> CascadedShadowPass::getLightSpaceMatrices()
{
    std::vector<glm::mat4> ret;
    for (size_t i = 0; i < m_shadowCascadeLevels.size() + 1; ++i)
    {
        if (i == 0)
        {
            ret.push_back(getLightSpaceMatrix(camera->nearPlane, m_shadowCascadeLevels[i]));
        }
        else if (i < m_shadowCascadeLevels.size())
        {
            ret.push_back(getLightSpaceMatrix(m_shadowCascadeLevels[i - 1], m_shadowCascadeLevels[i]));
        }
        else
        {
            ret.push_back(getLightSpaceMatrix(m_shadowCascadeLevels[i - 1], camera->farPlane));
        }
    }
    return ret;
}

std::vector<float> computeCascadeSplits(float nearPlane, float farPlane, size_t cascadeCount)
{
    std::vector<float> splits;
    splits.reserve(cascadeCount);
    float lambda = 1.0f; // blend factor between logarithmic and uniform split
    for (size_t i = 1; i <= cascadeCount; ++i)
    {
        float si = static_cast<float>(i) / static_cast<float>(cascadeCount);
        float logSplit = nearPlane * std::pow(farPlane / nearPlane, si);
        float uniformSplit = nearPlane + (farPlane - nearPlane) * si;
        float splitDist = glm::mix(uniformSplit, logSplit, lambda);
        splits.push_back(splitDist);
    }
    return splits;
}
