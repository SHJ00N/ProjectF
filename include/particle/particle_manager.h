#pragma once

#include <glm/glm.hpp>
#include <vector>

class ParticleManager
{
public:
    static ParticleManager *Instance;

    void Update(float dt);
    void Render(int width, int height);
    
    static class BloodParticle& SpawnBloodParticle(class Entity *parent, const glm::vec3 &localPos);
    static void Remove(class Particle *particle);

private:
    std::vector<class Particle*> m_particles;
};