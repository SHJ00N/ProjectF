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
std::map<std::string, TerrainTexture> ResourceManager::TerrainTextures;
std::map<std::string, ComputeShader> ResourceManager::ComputeShaders;


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

ComputeShader ResourceManager::LoadComputeShader(const char *computeShaderFile, std::string name)
{
    if(ComputeShaders.find(name) == ComputeShaders.end())
        ComputeShaders[name] = loadComputeShaderFromFile(computeShaderFile);
    return ComputeShaders[name];
}

ComputeShader& ResourceManager::GetComputeShader(std::string name)
{
    return ComputeShaders[name];
}

Texture2D ResourceManager::LoadTexture(const char *file, bool gamma, std::string name, unsigned int wrap_s, unsigned int wrap_t, unsigned int filter_min, unsigned int filter_max)
{
    if(Textures.find(name) == Textures.end())
        Textures[name] = loadTextureFromFile(file, gamma, wrap_s, wrap_t, filter_min, filter_max);
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

Animation ResourceManager::LoadAnimation(const char *file, Model &model, std::string name, bool isLooping)
{
    if(Animations.find(name) == Animations.end())
        Animations[name] = loadAnimationFromFile(file, model, isLooping);
    return Animations[name];
}

Animation& ResourceManager::GetAnimation(std::string name)
{
    return Animations[name];
}

TerrainTexture ResourceManager::LoadTerrainTexture(const char *diffuseFile, const char *normalFile, const char *roughFile, std::string name, bool gamma)
{
    if(TerrainTextures.find(name) == TerrainTextures.end())
        TerrainTextures[name] = loadTerrainTextureFromFile(diffuseFile, normalFile, roughFile, gamma);
    return TerrainTextures[name];
}

TerrainTexture& ResourceManager::GetTerrainTexture(std::string name)
{
    return TerrainTextures[name];
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
    // delet all terrain texture
    for (auto iter : TerrainTextures)
        iter.second.Clear();
    // delete all compute shaders
    for (auto iter : ComputeShaders)
        glDeleteProgram(iter.second.ID);

    // clear maps
    Shaders.clear();
    Textures.clear();
    Models.clear();
    TerrainTextures.clear();
    ComputeShaders.clear();
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

ComputeShader ResourceManager::loadComputeShaderFromFile(const char *computeShaderFile)
{
    // 1. retrieve the compute shader source code from filePath
    std::string computeCode;
    try
    {
        // open file
        std::ifstream computeShaderFile(computeShaderFile);
        std::stringstream cShaderStream;
        // read file's buffer contents into stream
        cShaderStream << computeShaderFile.rdbuf();
        // close file handler
        computeShaderFile.close();
        // convert stream into string
        computeCode = cShaderStream.str();
    }
    catch (std::exception e)
    {
        std::cout << "ERROR::SHADER: Failed to read compute shader file" << std::endl;
    }
    const char *cShaderCode = computeCode.c_str();
    // 2. now create shader object from source code
    ComputeShader shader;
    shader.Compile(cShaderCode);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char *file, bool gamma, unsigned int wrap_s, unsigned int wrap_t, unsigned int filter_min, unsigned int filter_max)
{
    // create texture object
    Texture2D texture;

    // load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);

    if(nrChannels == 1){
        texture.Internal_Format = texture.Image_Format = GL_RED;
    }
    else if(nrChannels == 3){
        texture.Internal_Format = gamma ? GL_SRGB : GL_RGB;
        texture.Image_Format = GL_RGB;
    }
    else if(nrChannels == 4){
        texture.Internal_Format = gamma ? GL_SRGB_ALPHA : GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    texture.Wrap_S = wrap_s;
    texture.Wrap_T = wrap_t;
    texture.Filter_Min = filter_min;
    texture.Filter_Max = filter_max;
    
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

Animation ResourceManager::loadAnimationFromFile(const char *file, Model &model, bool isLooping)
{
    // create animation object
    Animation animation(file, model, isLooping);
    return animation;
}

TerrainTexture ResourceManager::loadTerrainTextureFromFile(const char *diffuseFile, const char *normalFile, const char *roughFile, bool gamma)
{
    TerrainTexture terrainTexture(diffuseFile, normalFile, roughFile, gamma);
    return terrainTexture;
}