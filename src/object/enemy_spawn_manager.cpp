#include "object/enemy/enemy_spawn_manager.h"
#include "scene/scene.h"
#include "object/enemy/enemy.h"
#include "resource_manager.h"

#include <iostream>
#include <sstream>

void EnemySpawnManager::LoadSpawnDataFromFile(const std::string &path)
{
    std::ifstream file(path);
    std::string line;

    while(std::getline(file, line))
    {
        if(line.empty()) continue;
        SpawnData data;

        // position
        std::stringstream ss(line);
        std::string str;
        ss >> str;
        ss >> data.position.x >> data.position.y >> data.position.z;

        // rotation
        getline(file, line);
        std::stringstream rot(line);
        rot >> str;
        rot >> data.rotation.x >> data.rotation.y >> data.rotation.z;
        // waypoints
        int wayPointCount;
        ss >> wayPointCount;

        for(int i = 0; i < wayPointCount; i++)
        {
            std::getline(file, line);
            std::stringstream wp(line);

            glm::vec3 point;
            wp >> point.x >> point.y >> point.z;

            data.wayPoints.push_back(point);
        }

        // respawnTime
        std::getline(file, line);
        std::stringstream resp(line);
        resp >> str;
        resp >> data.respawnTime;


        SpawnRunTime runTimeData;
        runTimeData.data = data;
        m_spawns.push_back(runTimeData);
    }
}

void EnemySpawnManager::Init(Scene *scene, GameObject& target)
{
    for(auto& spawn : m_spawns)
    {
        spawnEnemy(scene, target, spawn);
    }
}

void EnemySpawnManager::Update(Scene *scene, GameObject& target, float dt)
{
    for(auto &spawn : m_spawns)
    {
        if(spawn.currentEnemy)
        {
            if(spawn.currentEnemy->IsDeath)
            {
                spawn.currentEnemy = nullptr;

                if(spawn.data.respawnTime > 0.0f)
                {
                    spawn.isSpawn = false;
                    spawn.spawnTimer = 0.0f;
                }
            }

            continue;
        }

        if(!spawn.isSpawn)
        {
            spawn.spawnTimer += dt;

            if(spawn.spawnTimer >= spawn.data.respawnTime)
            {
                spawnEnemy(scene, target, spawn);
            }
        }
    }
}

void EnemySpawnManager::spawnEnemy(Scene *scene, GameObject &target, SpawnRunTime &spawn)
{
    if(spawn.currentEnemy) return;
    
     // spawn enemy
    auto &enemy = scene->Root->addChild<Enemy>(ResourceManager::GetModel("enemy"), ResourceManager::GetShader("boneModel"), spawn.data.position, glm::vec3(0.01f), spawn.data.rotation);
    enemy.AddAnimation("Idle", &ResourceManager::GetAnimation("enemy_idle"));
    enemy.AddAnimation("Walk", &ResourceManager::GetAnimation("enemy_walk"));
    enemy.AddAnimation("Attack", &ResourceManager::GetAnimation("enemy_attack1"));
    enemy.AddAnimation("Hit", &ResourceManager::GetAnimation("enemy_hit"));
    enemy.AddAnimation("Death", &ResourceManager::GetAnimation("enemy_death"));
    enemy.AddAnimation("Run", &ResourceManager::GetAnimation("enemy_run"));
    enemy.SetupBehaviorTree(spawn.data.wayPoints, target);
    enemy.EquipWeapon(ResourceManager::GetModel("sword"), ResourceManager::GetShader("staticModel"), glm::vec3(0.0f), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    enemy.Init();

    // regist enemy
    scene->GetRenderables().push_back(&enemy);
    scene->GetGameObjects().push_back(&enemy);
    scene->GetCollidables().push_back(&enemy);

    // update spawn runtime data
    spawn.currentEnemy = &enemy;
    spawn.spawnTimer = 0.0f;
    spawn.isSpawn = true;
}