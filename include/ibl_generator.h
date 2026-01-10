#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct IBLData
{
    unsigned int envCubeMap = 0;
    unsigned int irradianceMap = 0;
    unsigned int prefilterMap = 0;

    void Destroy()
    {
        if(envCubeMap) glDeleteTextures(1, &envCubeMap);
        if(irradianceMap) glDeleteTextures(1, &irradianceMap);
        if(prefilterMap) glDeleteTextures(1, &prefilterMap);
        envCubeMap = irradianceMap = prefilterMap = 0;
    }
};

// IBL texture resoultion
const unsigned int ENV_SIZE = 1024;
const unsigned int DIF_SIZE = 32;
const unsigned int SPE_SIZE = 128;
const unsigned int LUT_SIZE = 512;
const unsigned int MIP_SIZE = 5;

void loadIBLShaders();
void loadBrdfShader();
unsigned int loadFromeTextureFile(const char *path);

class IBLGenerator
{
public:
    // IBL generate functions
    static IBLData GenerateIBLFromHDR(const char *HDRPath);
    static unsigned int GenerateBRDFLUT();
    // de-allocates VAO, VBO
    static void Clear();
private:
    IBLGenerator() { };

    static unsigned int cubeVAO;
    static unsigned int cubeVBO;
    static unsigned int quadVAO;
    static unsigned int quadVBO;

    static void renderCube();
    static void renderQuad();
};