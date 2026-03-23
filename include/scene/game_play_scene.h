#pragma once

#include <vector>
#include <string>

#include "scene/scene.h"
#include "camera.h"

class GamePlayScene : public Scene
{
public: 
    // constructor(s)
    GamePlayScene(unsigned int width, unsigned int height);
    ~GamePlayScene() override;

    // scene's memberfunction overriding
    void Init() override;
    void Start() override;
    void Update(float dt) override;
    void ProcessInput(float dt) override;
    void End() override;
    void UIUpdate() override;

private:
    // obj
    class Player* player;
    class Enemy* enemy;
    class BoneDemoObj *boneDemoObj;
    class World *world;
    class Weapon *sword;
    class CollisionSystem *collisionSystem;
    class EnemySpawnManager *enemySpawnManager;
};