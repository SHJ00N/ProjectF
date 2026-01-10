#include "resource_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "stb_image.h"

// Instantiate static variables
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;
std::map<std::string, Model>        ResourceManager::Models;
std::map<std::string, Animation>    ResourceManager::Animations;
std::map<std::string, Terrain>      ResourceManager::Terrains;

Shader ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, const char *tcShaderFile, const char *teShaderFile, std::string name)
{
    if(Shaders.find(name) == Shaders.end())
        Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile, tcShaderFile, teShaderFile);
    return Shaders[name];
}

Shader& ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char *file, bool alpha, std::string name)
{
    if(Textures.find(name) == Textures.end())
        Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
}

Texture2D& ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

Model ResourceManager::LoadModel(const char *file, bool gamma, std::string name)
{
    if(Models.find(name) == Models.end())
        Models[name] = loadModelFromFile(file, gamma);
    return Models[name];
}

Model& ResourceManager::GetModel(std::string name)
{
    return Models[name];
}

Animation ResourceManager::LoadAnimation(const char *file, Model &model, std::string name)
{
    if(Animations.find(name) == Animations.end())
        Animations[name] = loadAnimationFromFile(file, model);
    return Animations[name];
}

Animation& ResourceManager::GetAnimation(std::string name)
{
    return Animations[name];
}

Terrain ResourceManager::LoadTerrain(const char *diffuseFile, const char *heightFile, const char *normalFile, const char *roughFile, std::string name, float heightScale, float worldScale, unsigned int rez)
{
    if(Terrains.find(name) == Terrains.end())
        Terrains[name] = loadTerrainFromFile(diffuseFile, heightFile, normalFile, roughFile, heightScale, worldScale, rez);
    return Terrains[name];
}

Terrain& ResourceManager::GetTerrain(std::string name)
{
    return Terrains[name];
}

void ResourceManager::Clear()
{
    // (properly) delete all shaders	
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
    // (properly) delete all textures
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
    // delete all models
    for (auto iter : Models)
        iter.second.Clear();
    // delete all animations
    Animations.clear();
    // delete all terrains
    for (auto iter : Terrains)
        iter.second.Clear();

    // clear maps
    Shaders.clear();
    Textures.clear();
    Models.clear();
    Terrains.clear();
}

Shader ResourceManager::loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, const char *tcShaderFile, const char *teShaderFile)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::string tessControlCode;
    std::string tessEvalCode;
    try
    {
        // open files
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // if geometry shader path is present, also load a geometry shader
        if (gShaderFile != nullptr)
        {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
        // tessellation control shader
        if (tcShaderFile != nullptr)
        {
            std::ifstream tessControlShaderFile(tcShaderFile);
            std::stringstream tcShaderStream;
            tcShaderStream << tessControlShaderFile.rdbuf();
            tessControlShaderFile.close();
            tessControlCode = tcShaderStream.str();
        }
        // tessellation evaluation shader
        if (teShaderFile != nullptr)
        {
            std::ifstream tessEvalShaderFile(teShaderFile);
            std::stringstream teShaderStream;
            teShaderStream << tessEvalShaderFile.rdbuf();
            tessEvalShaderFile.close();
            tessEvalCode = teShaderStream.str();
        }
    }
    catch (std::exception e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    const char *gShaderCode = geometryCode.c_str();
    const char *tcShaderCode = tessControlCode.c_str();
    const char *teShaderCode = tessEvalCode.c_str();
    // 2. now create shader object from source code
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr, tcShaderFile != nullptr ? tcShaderCode : nullptr, teShaderFile != nullptr ? teShaderCode : nullptr);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char *file, bool alpha)
{
    // create texture object
    Texture2D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    // now generate texture
    texture.Generate(width, height, data);
    // and finally free image data
    stbi_image_free(data);
    return texture;
}

Model ResourceManager::loadModelFromFile(const char *file, bool gamma)
{
    // create model object
    Model model;
    model.LoadModel(file, gamma);
    return model;
}

Animation ResourceManager::loadAnimationFromFile(const char *file, Model &model)
{
    // create animation object
    Animation animation(file, model);
    return animation;
}

Terrain ResourceManager::loadTerrainFromFile(const char *diffuseFile, const char *heightFile, const char *normalFile, const char *roughFile, float heightScale, float worldScale, unsigned int rez)
{
    // create terrain object
    Terrain terrain(diffuseFile, heightFile, normalFile, roughFile, heightScale, worldScale, rez);
    return terrain;
}