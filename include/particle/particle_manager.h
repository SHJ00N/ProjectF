#pragma once

#include <glm/glm.hpp>
#include <vector>

class ParticleManager
{
public:
    static ParticleManager& GetInstance()
    {
        static ParticleManager instance;
        return instance;
    }

    void Update(float dt);
    void Render(int width, int height);
    
    class BloodParticle& SpawnBloodParticle(class Entity *parent, const glm::vec3 &localPos);
    void Remove(class Particle *particle);

private:
    ParticleManager() = default;
    ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;
	ParticleManager(ParticleManager&&) = delete;
	ParticleManager& operator=(ParticleManager&&) = delete;

    std::vector<class Particle*> m_particles;
};