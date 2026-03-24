#pragma once

#include <glad/glad.h>

class Scene;

class ParticlePass
{
public:
    void Render(unsigned int framebuffer, int width, int height);
};