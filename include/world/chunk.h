#pragma once

#include <vector>
#include <memory>
#include "object/collider/aabb.h"
#include "object/transform.h"

const unsigned int NUM_PATCH_PTS = 4;
const unsigned int NUM_VERTICES_ATTRIBUTE = 5; // x, y, z, u, v

class Shader;

class Chunk
{
public:
    Transform transform;
    // constructor and destructor
    Chunk(int chunkX, int chunkZ, int chunkRez, float chunkWorldSize);
    ~Chunk();

    // generate vertices
    void BuildMesh();
    // generate bound
    void BuildBound(const struct HeightMapData &data, const float heightScale);

    // render function
    void Render(Shader &shader, const struct Frustum &frustum);
    void RenderShadow(Shader &shader);

private:
    // gpu resources
    unsigned int m_VBO, m_VAO, m_EBO;
    // chunk info
    int m_chunkX, m_chunkZ, m_chunkRez;
    float m_chunkWorldSize;
    // collider for frustum culling
    std::unique_ptr<AABB> m_chunkBound;
    // mesh
    std::vector<float> m_vertices;
    std::vector<uint32_t> m_indices;

    // update vao, vbo
    void setUpMesh();
};