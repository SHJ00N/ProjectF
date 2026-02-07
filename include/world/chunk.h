#pragma once

#include <vector>

const unsigned int NUM_PATCH_PTS = 4;
const unsigned int NUM_VERTICES_ATTRIBUTE = 5; // x, y, z, u, v

class Shader;

class Chunk
{
public:
    // constructor and destructor
    Chunk(int chunkX, int chunkZ, int chunkRez, float chunkWorldSize);
    ~Chunk();

    // generate vertices
    void BuildMesh();

    // render function
    void Render(Shader &shader);
    void RenderShadow(Shader &shader);

private:
    // gpu resources
    unsigned int m_VBO, m_VAO, m_EBO;
    // chunk info
    int m_chunkX, m_chunkZ, m_chunkRez;
    float m_chunkWorldSize;
    // mesh
    std::vector<float> m_vertices;
    std::vector<uint32_t> m_indices;

    // update vao, vbo
    void setUpMesh();
};