#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <memory>

#include "scene.h"

class Game
{
public:
    // screen size
    unsigned int Width, Height;

    // constructor(s)
    Game(unsigned int width, unsigned int height);
    ~Game();

    // initialize game state (load all resources etc.)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render(float dt);

    // getter and setter
    Scene* GetCurrentScene();

private:
    std::vector<std::unique_ptr<Scene>> m_sceneStack;

    // process scene request
    void processSceneRequest();
    // create scene based on scene request
    std::unique_ptr<Scene> createScene(unsigned int sceneID);
};