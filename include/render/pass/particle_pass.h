#pragma once

#include <glad/glad.h>

class Scene;

class ParticlePass
{
public:
    void Render(Scene *scene, unsigned int framebuffer, int width, int height);
};