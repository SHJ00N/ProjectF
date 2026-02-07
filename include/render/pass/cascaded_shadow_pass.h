#pragma once

#include <glad/glad.h>
#include <vector>

#include "camera.h"
#include "render/shadow_map_offset_texture.h"

class Scene;

const unsigned int SHADOW_MAP_RESOLUTION = 4096;
const unsigned int CASCADE_SIZE = 4;

struct CSMatricesUBO
{
    glm::mat4x4 lightSpaceMatrices[16];
};

struct CSMdataUBO
{
    float cascadePlaneDistances[CASCADE_SIZE * 4];
    int cascadeCount;
    glm::vec3 padding0;
};

struct ShadowMapTexture
{
    unsigned int shadowMapTexture;
    ShadowMapOffsetTextureData shadowMapOffsetTexture;
};

class CascadedShadowPass
{
public:
    //constructor(s)
    CascadedShadowPass(unsigned int width, unsigned int height);
    ~CascadedShadowPass();

    // functions
    void Configure(Scene *scene);
    void Render(Scene *scene);

    // getter and setter
    ShadowMapTexture GetShadowMapTexture() { return {m_shadowMap, m_offsetTexture->GetData()}; }
    void SetLightDirection(const glm::vec3 &dir) { m_lightDir = dir; }

private:
    unsigned int m_matricesUBO, m_csmDataUBO;
    unsigned int m_shadowMapFBO;
    unsigned int m_shadowMap;

    // shadow offset texture for PCF
    ShadowMapOffsetTexture *m_offsetTexture;

    // screen size
    unsigned int m_width, m_height;
    Camera *camera; // pointer to the main camera

    // shadow cascade levels
    std::vector<float> m_shadowCascadeLevels;
    glm::vec3 m_lightDir;

    // cascaded shadow helper functions
    std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4 &proj, const glm::mat4 &view);
    glm::mat4 getLightSpaceMatrix(float nearPlane, float farPlane);
    std::vector<glm::mat4> getLightSpaceMatrices();
};