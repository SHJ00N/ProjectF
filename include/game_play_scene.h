#pragma once

#include <vector>
#include <string>

#include "scene.h"
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
    void Render(float dt) override;
    void End() override;
};