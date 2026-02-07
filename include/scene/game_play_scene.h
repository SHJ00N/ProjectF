#pragma once

#include <vector>
#include <string>

#include "scene/scene.h"
#include "camera.h"

class Player;
class World;
class BoneDemoObj;

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

private:
    // obj
    Player *player;
    BoneDemoObj *boneDemoObj;
    World *world;
};