#include "terrain_renderer.h"
#include "resource_manager.h"

TerrainRenderer::TerrainRenderer()
{
    ResourceManager::LoadShader("shaders/csm_shader/terrainShadow.vert", "shaders/csm_shader/cascadedShadow.frag", "shaders/csm_shader/cascadedShadow.geom", "shaders/csm_shader/terrainShadow.tesc", "shaders/csm_shader/terrainShadow.tese", "terrainCSMShader");
}

void TerrainRenderer::Draw(Shader &shader, Transform &transform, Terrain &terrain)
{
    // set model matrix
    shader.Use();
    shader.SetMatrix4("model", transform.GetModelMatrix());
    // set textures and render terrain
    terrain.Render(shader);
}

void TerrainRenderer::DrawShadow(Transform &transform, Terrain &terrain)
{
    Shader &shadowShader = ResourceManager::GetShader("terrainCSMShader");
    // set model matrix
    shadowShader.Use();
    shadowShader.SetMatrix4("model", transform.GetModelMatrix());
    // render terrain
    terrain.RenderShadow(shadowShader);
}