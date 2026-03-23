#include "particle/blood_particle.h"
#include "shader.h"
#include "compute_shader.h"
#include "texture.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

BloodParticle::BloodParticle(Shader &shader, ComputeShader &computeShader, Texture2D& texture, glm::vec3 position, glm::vec3 scale, int numParticlesX, int numParticlesY, int numParticlesZ)
: Particle(shader, computeShader, position, scale, numParticlesX, numParticlesY, numParticlesZ), m_texture(texture)
{
    m_noiseTexture.Generate(16, 16);
}

BloodParticle::~BloodParticle()
{
    if(m_quadVBO != 0) glDeleteBuffers(1, &m_quadVBO);
    if(m_quadVAO != 0) glDeleteVertexArrays(1, &m_quadVAO);
}

void BloodParticle::Init()
{
    updateSelfAndChild();
    // calculate initial positions
    std::vector<glm::vec4> positions(m_totalParticles);
    calculatePositions(positions);

    // initialize buffers
    InitBuffers(positions);

    // configure quad buffer
    float quadVertices[] =
    {
        // pos      // uv
        -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, 1.0f, 1.0f,

        -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);

    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),(void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, m_posBuf);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);

    glVertexAttribDivisor(2, 1);

    // set shader's uniform value
    m_shader.Use();
    m_shader.SetInteger("particleImage", 0);
    m_shader.SetInteger("noiseTexture", 1);
}

void BloodParticle::Update(float dt)
{
    m_lifeTime -= dt;
    if(m_lifeTime <= 0.0f) EntityDestroyed = true;
    
    m_computeShader.Use();
    m_computeShader.SetFloat("dt", dt);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_posBuf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_velBuf);

    glDispatchCompute(m_totalParticles, 1, 1);
}

void BloodParticle::Render(int width, int height)
{
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    m_shader.Use();
    m_shader.SetMatrix4("model", transform.GetModelMatrix());
    m_shader.SetFloat("screenWidth", (float)width);
    m_shader.SetFloat("screenHeight", (float)height);

    m_texture.Bind(0);
    m_noiseTexture.Bind(1);

    glBindVertexArray(m_quadVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_totalParticles);
    glBindVertexArray(0);
}

void BloodParticle::calculatePositions(std::vector<glm::vec4> &positions)
{
    for (int i = 0; i < m_totalParticles; ++i)
    {
        positions[i] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}
