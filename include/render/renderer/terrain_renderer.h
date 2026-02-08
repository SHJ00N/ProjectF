#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "render/slope_lighter_texture.h"

class Shader;
class World;
class TerrainTexture;

class TerrainRenderer
{
public:
    TerrainRenderer(Shader &shader, Shader &shadowShader, World &world, TerrainTexture &terrainTexture);

    void Render(const struct Frustum &frustum) const;
    void RenderShadow() const;

private:
    // reference
    Shader &m_shader;
    Shader &m_shadowShader;
    World &m_world;
    TerrainTexture &m_terrainTexture;
    // member
    std::unique_ptr<SlopeLighterTexture> m_slopeLighter;
};