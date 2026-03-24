#include "particle/particle_manager.h"
#include "particle/particle.h"
#include "particle/blood_particle.h"
#include "object/entity.h"
#include "resource_manager.h"

#include <algorithm>

void ParticleManager::Update(float dt)
{
    for(auto& particle : m_particles)
    {
        particle->Update(dt);
    }
}

void ParticleManager::Render(int width, int height)
{
    for(auto& particle: m_particles)
    {
        particle->Render(width, height);
    }
}

void ParticleManager::Remove(Particle *particle)
{
    auto& list = m_particles;
    list.erase(std::remove(list.begin(), list.end(), particle), list.end());
}

BloodParticle& ParticleManager::SpawnBloodParticle(Entity *parent, const glm::vec3 &localPos)
{
    auto& blood = parent->addChild<BloodParticle>(ResourceManager::GetShader("blood"), ResourceManager::GetComputeShader("bloodCompute"), ResourceManager::GetTexture("blood_splatter"), 
            localPos, glm::vec3(1.0f), 1, 1, 1);
    blood.Init();
    m_particles.push_back(&blood);

    return blood;
}