#include "world/chunk.h"
#include "shader.h"
#include "frustum.h"
#include "world/world_structures.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Chunk::Chunk(int chunkX, int chunkZ, int chunkRez, float chunkWorldSize) : m_chunkX(chunkX), m_chunkZ(chunkZ), m_chunkRez(chunkRez), m_chunkWorldSize(chunkWorldSize)
{
    // set chunk position
    glm::vec3 position = glm::vec3(m_chunkX * m_chunkWorldSize, 0.0f, m_chunkZ * m_chunkWorldSize);
    transform.SetLocalPosition(position);
}

Chunk::~Chunk()
{
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void Chunk::BuildMesh()
{   
    // generate vertices
    m_vertices.clear();
    m_vertices.reserve((m_chunkRez + 1) * (m_chunkRez + 1) * NUM_VERTICES_ATTRIBUTE);
    for(int i = 0; i <= m_chunkRez; ++i)
    {
        for(int j = 0; j <= m_chunkRez; ++j)
        {
            float u = j / static_cast<float>(m_chunkRez);
            float v = i / static_cast<float>(m_chunkRez);

            float x = u * m_chunkWorldSize;
            float z = v * m_chunkWorldSize;

            // x, y, z, u, v
            m_vertices.push_back(x);
            m_vertices.push_back(0.0f);
            m_vertices.push_back(z);
            m_vertices.push_back(u);
            m_vertices.push_back(v);
        }
    }
    // configure indices
    m_indices.clear();
    m_indices.reserve(m_chunkRez * m_chunkRez * 6); // quad = 2 * triangle(3)
    for(int i = 0; i < m_chunkRez; ++i)
    {
        for(int j = 0; j < m_chunkRez; ++j)
        {
            int p00 = i * (m_chunkRez + 1) + j;
            int p01 = p00 + 1;
            int p10 = (i + 1) * (m_chunkRez + 1) + j;
            int p11 = p10 + 1;

            m_indices.push_back(p00);
            m_indices.push_back(p10);
            m_indices.push_back(p01);
            m_indices.push_back(p11);
        }
    }

    // update buffers
    setUpMesh();
}

void Chunk::BuildBound(const HeightMapData &data, const float heightScale)
{
    unsigned short minH = std::numeric_limits<unsigned short>::max();
    unsigned short maxH = 0;

    // calculate min, max height
    for(int i = 0; i < m_chunkWorldSize; ++i)
    {
        for(int j = 0; j < m_chunkWorldSize; ++j)
        {
            int x = m_chunkX * m_chunkWorldSize + j;
            int z = m_chunkZ * m_chunkWorldSize + i;

            // check out of range
            if(x >= 0 && x < data.heightMapWidth && z >= 0 && z < data.heightMapHeight)
            {
                unsigned short height = data.heights[z * data.heightMapWidth + x];
                
                minH = std::min(minH, height);
                maxH = std::max(maxH, height);
            }
        }
    }

    // convert to min, max height of real world
    float minY = static_cast<float>(minH) / 65535.0f * heightScale;
    float maxY = static_cast<float>(maxH) / 65535.0f * heightScale;

    glm::vec3 center = glm::vec3(m_chunkWorldSize * 0.5f, (minY + maxY) * 0.5f, m_chunkWorldSize * 0.5f);

    // create bound
    m_chunkBound = std::make_unique<AABB>(center, m_chunkWorldSize * 0.5f, (maxY - minY) * 0.5f, m_chunkWorldSize * 0.5f);
}

void Chunk::Render(Shader &shader, const Frustum &frustum)
{
    // update transform
    if(transform.IsDirty()) transform.ComputeModelMatrix();
    // frustum culling
    if(m_chunkBound->isOnFrustum(frustum, transform))
    {
        // set uniform
        shader.SetMatrix4("model", transform.GetModelMatrix());

        glBindVertexArray(m_VAO);
        glDrawElements(GL_PATCHES, m_indices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }   
}

void Chunk::RenderShadow(Shader &shader)
{
    // set model matrix
    if(transform.IsDirty()) transform.ComputeModelMatrix();
    shader.SetMatrix4("model", transform.GetModelMatrix());

    glBindVertexArray(m_VAO);
    glDrawElements(GL_PATCHES, m_indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void Chunk::setUpMesh()
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // VBO
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), &m_vertices[0], GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    // EBO
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(int), &m_indices[0], GL_STATIC_DRAW);

    // set tessellation patch size
    glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

    glBindVertexArray(0);
}