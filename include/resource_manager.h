#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <glad/glad.h>

#include "texture.h"
#include "shader.h"
#include "model.h"
#include "animation.h"
#include "terrain.h"

class ResourceManager
{
public:
    // resource storage
    static std::map<std::string, Shader>    Shaders;
    static std::map<std::string, Texture2D> Textures;
    static std::map<std::string, Model>     Models;
    static std::map<std::string, Animation> Animations;
    static std::map<std::string, Terrain>   Terrains;
    // loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
    static Shader    LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, const char *tcShaderFile, const char *teShaderFile, std::string name);
    // retrieves a stored sader
    static Shader&    GetShader(std::string name);
    // loads (and generates) a texture from file
    static Texture2D LoadTexture(const char *file, bool alpha, std::string name);
    // retrieves a stored texture
    static Texture2D& GetTexture(std::string name);
    // loads (and generates) a model from file
    static Model LoadModel(const char *file, bool gamma, std::string name);
    // retrieves a stored model
    static Model& GetModel(std::string name);
    // loads (and generates) an animation from file
    static Animation LoadAnimation(const char *file, Model &model, std::string name);
    // retrieves a stored animation
    static Animation& GetAnimation(std::string name);
    // loads (and generates) terrain from file
    static Terrain LoadTerrain(const char *diffuseFile, const char *heightFile, const char *normalFile, const char *roughFile, std::string name, float heightScale = 1.0f, float worldScale = 1.0f, unsigned int rez = 20);
    // retrieves a stored terrain
    static Terrain& GetTerrain(std::string name);
    // properly de-allocates all loaded resources
    static void      Clear();
private:
    // private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
    ResourceManager() { }
    // loads and generates a shader from file
    static Shader    loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile = nullptr, const char *tcShaderFile = nullptr, const char *teShaderFile = nullptr);
    // loads a single texture from file
    static Texture2D loadTextureFromFile(const char *file, bool alpha);
    // loads a single model from file
    static Model loadModelFromFile(const char *file, bool gamma);
    // loads a single animation from file
    static Animation loadAnimationFromFile(const char *file, Model &model);
    // loads a single terrain from file
    static Terrain loadTerrainFromFile(const char *diffuseFile, const char *heightFile, const char *normalFile, const char *roughFile, float heightScale, float worldScale, unsigned int rez);
};

#endif