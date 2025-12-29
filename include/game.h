#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera.h"

class Game
{
public:
    bool Keys[1024];
    bool KeysProcessed[1024];
    unsigned int Width, Height;
    Camera DefaultCamera;

    Game(unsigned int width, unsigned int height);
    ~Game();

    // initialize game state (load all resources etc.)
    void Init();

    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render(float dt);
};