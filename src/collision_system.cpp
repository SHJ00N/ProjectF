#include "collision_system.h"
#include "world/world.h"
#include "object/interface/collidable.h"
#include "object/game_object.h"

#include <iostream>

void CollisionSystem::Step(const World& world)
{
    broadPhase(world);
    narrowPhase();
    dispatchCollisionEvents();
}

void CollisionSystem::broadPhase(const World& world)
{
    auto *chunkManager = world.GetChunkManager();
    auto chunkList = chunkManager->GetPhysicsChunkList();

    m_pairs.clear();
    m_allColliders.clear(); 
    for(Chunk* chunk : chunkList)
    {
        ChunkCoord baseCoord = chunk->GetChunkCoord();
        auto &baseColliders = chunk->GetColliders();

        // check internal colliders
        for(int i = 0; i < baseColliders.size(); ++i)
        {
            for(int j = i + 1; j < baseColliders.size(); ++j)
            {
                m_pairs.emplace(baseColliders[i], baseColliders[j]);
            }
            m_allColliders.insert(baseColliders[i]);
        }

        // neighbor offset
        static const ChunkCoord offsets[4] = 
        {
            {1, 0}, {0, 1}, {1, 1}, {-1, 1}
        };

        // check neighbor with base
        for(const auto& offset : offsets)
        {
            ChunkCoord neighborCoord = {baseCoord.x + offset.x, baseCoord.z + offset.z};

            Chunk* neighborChunk = chunkManager->GetChunk(neighborCoord);
            if(!neighborChunk) continue;

            auto &neighborColliders = neighborChunk->GetColliders();
            for(Collider* colliderA : baseColliders)
            {
                for(Collider* colliderB : neighborColliders)
                {
                    m_pairs.emplace(colliderA, colliderB);
                    m_allColliders.insert(colliderB);
                }
            }
        }
    }
}

void CollisionSystem::narrowPhase()
{
    // clear collision list
    for(Collider* collider : m_allColliders)
    {
        collider->BeginFrame();
    }

    for(auto& pair : m_pairs)
    {
        Collider *colliderA = pair.a;
        Collider *colliderB = pair.b;

        // if collides, add collider to object
        if(colliderA->isIntersectsAABB(*colliderB))
        {
            colliderA->AddCollision(colliderB);
            colliderB->AddCollision(colliderA);
        }
    }
}

void CollisionSystem::dispatchCollisionEvents()
{
    for(Collider* collider : m_allColliders)
    {
        auto *owner = collider->Owner;
        if(!owner) 
        {
            std::cout <<"owner null!" <<std::endl;
            continue;
        }

        Collidable *collidable = dynamic_cast<Collidable*>(owner);
        if(!collidable){
            std::cout << "collidable null!" << std::endl;
            continue;
        } 

        auto& current = collider->GetCurrentCollisions();
        auto& previous = collider->GetPreviousCollisions();

        // check enter and stay
        for(Collider *other : current)
        {
            auto it = std::find(previous.begin(), previous.end(), other);

            if(it == previous.end())
            {
                collidable->OnCollisionEnter(other);
            } 
            else 
            {
                collidable->OnCollisionStay(other);
            }
        }

        // check collision exit
        for(Collider *other : previous)
        {
            auto it = std::find(current.begin(), current.end(), other);
            if(it == current.end())
            {
                collidable->OnCollisionExit(other);
            }
        }
    }
}