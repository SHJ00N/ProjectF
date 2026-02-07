#include "render/renderer/terrain_renderer.h"
#include "shader.h"
#include "world/world.h"
#include "render/terrain_texture.h"
#include "world/chunk.h"

TerrainRenderer::TerrainRenderer(Shader &shader, Shader &shadowShader, World &world, TerrainTexture &terrainTexture)
: m_shader(shader), m_shadowShader(shadowShader), m_world(world), m_terrainTexture(terrainTexture)
{
    m_slopeLighter = std::make_unique<SlopeLighterTexture>(world.GetHeightMapData(), world.GetHeightScale());
}

void TerrainRenderer::Render() const
{
    m_shader.Use();
    m_shader.SetInteger("heightMap", 0);
    m_shader.SetInteger("texture_diffuse", 1);
    m_shader.SetInteger("texture_normal", 2);
    m_shader.SetInteger("texture_roughness", 3);
    m_shader.SetInteger("texture_slopeLighter", 4);
    m_shader.SetFloat("heightScale", m_world.GetHeightScale());
    m_shader.SetFloat("worldScale", m_world.GetWorldScale());
    m_shader.SetInteger("heightMapWidth", m_world.GetHeightMapWidth());
    m_shader.SetInteger("heightMapHeight", m_world.GetHeightMapHeight());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_world.GetHeightMap());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_terrainTexture.AlbedoMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_terrainTexture.NormalMap);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_terrainTexture.RoughnessMap);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_slopeLighter.get()->GetLighterMap());

    for(const auto &chunk : m_world.GetChunkManager()->GetChunkList())
    {
        chunk->Render(m_shader);
    }
}

void TerrainRenderer::RenderShadow() const
{
    m_shadowShader.Use();
    m_shadowShader.SetInteger("heightMap", 0);
    m_shadowShader.SetFloat("heightScale", m_world.GetHeightScale());
    m_shadowShader.SetFloat("worldScale", m_world.GetWorldScale());
    m_shadowShader.SetInteger("heightMapWidth", m_world.GetHeightMapWidth());
    m_shadowShader.SetInteger("heightMapHeight", m_world.GetHeightMapHeight());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_world.GetHeightMap());

    for(const auto &chunk : m_world.GetChunkManager()->GetChunkList())
    {
        chunk->RenderShadow(m_shadowShader);
    }
}