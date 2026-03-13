#pragma once

#include "particle/particle.h"
#include "noise_texture.h"

class Texture2D;

class BloodParticle : public Particle
{
public:
    BloodParticle(Shader &shader, ComputeShader &computeShader, Texture2D& texture, glm::vec3 position, glm::vec3 scale, int numParticlesX, int numParticlesY, int numParticlesZ);
    ~BloodParticle();

    void Init() final;
    void Update(float dt) final;
    void Render(unsigned int depthTexture, int width, int height) final;

private:
    float m_lifeTime = 1.5f;
    unsigned int m_quadVAO = 0;
    unsigned int m_quadVBO = 0;

    Texture2D &m_texture;
    NoiseTexture m_noiseTexture;
    
    void calculatePositions(std::vector<glm::vec4> &positions);
};