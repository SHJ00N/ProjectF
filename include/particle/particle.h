#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

#include "object/entity.h"
#include "object/interface/renderable.h"
#include "particle/particle_manager.h"

class Shader;
class ComputeShader;

class Particle : public Entity
{
public:
    Particle(Shader &shader, ComputeShader &computeShader, glm::vec3 position, glm::vec3 scale, int numParticlesX, int numParticlesY, int numParticlesZ)
    : m_shader(shader), m_computeShader(computeShader), m_numParticlesX(numParticlesX), m_numParticlesY(numParticlesY), m_numParticlesZ(numParticlesZ)
    {
        m_totalParticles = numParticlesX * numParticlesY * numParticlesZ;

        transform.SetLocalPosition(position);
        transform.SetLocalScale(scale);
    }
    ~Particle()
    {
        if (m_posBuf != 0) glDeleteBuffers(1, &m_posBuf);
        if (m_velBuf != 0) glDeleteBuffers(1, &m_velBuf);

        ParticleManager::GetInstance().Remove(this);
    }

    virtual void Init() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render(int width, int height) = 0;

protected:
    Shader &m_shader;
    ComputeShader &m_computeShader;
    unsigned int m_posBuf = 0;
    unsigned int m_velBuf = 0;

    int m_numParticlesX = 0;
    int m_numParticlesY = 0;
    int m_numParticlesZ = 0;

    int m_totalParticles = 0;

    void InitBuffers(std::vector<glm::vec4> &positions)
    {
        glCreateBuffers(1, &m_posBuf);
        glCreateBuffers(1, &m_velBuf);
        
        size_t size = (int)positions.size() * sizeof(positions[0]);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_posBuf);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, positions.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_posBuf);

        std::vector<glm::vec4> velocities(positions.size(), glm::vec4(0.0f));
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_velBuf);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, velocities.data(), GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_velBuf);
    }
};