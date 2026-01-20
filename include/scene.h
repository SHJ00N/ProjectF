#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "camera.h"
#include "light.h"
#include "ibl_generator.h"
#include "renderable.h"
#include "game_object.h"

// operator for scene management
enum class SceneOp { None, Push, Pop };
struct SceneRequest
{
    SceneOp op = SceneOp::None;
    unsigned int sceneID;
};

enum class RenderType { Forward, Deferred };

class Scene
{
public:
    // key processing members
    bool Keys[1024];
    bool KeysProcessed[1024];

    

    // constructor(s)
    Scene(){ };
    Scene(unsigned int width, unsigned int height) : Width(width), Height(height), Keys{0}, KeysProcessed{0} { };
    virtual ~Scene() = default;

    // init scene
    virtual void Init() = 0;
    // scene processing functions
    virtual void Start() = 0;
    virtual void Update(float dt) = 0;
    virtual void ProcessInput(float dt) = 0;
    virtual void End() = 0;
    
    // getter
    SceneRequest GetSceneRequest() { return Request; }
    Camera* GetCamera() { return MainCamera; }
    std::vector<Light*> GetLights() { return Lights; }
    RenderType GetRenderType() { return renderType; }
    IBLData GetIBLData() { return IBLtextures; }
    std::vector<Renderable*> GetRenderables() { return renderables; }
    // setter
    void RequestClear() { Request = {SceneOp::None, {}}; }

protected:
    // screen size
    unsigned int Width, Height;
    // all scenes except menu scene have a main camera 
    Camera *MainCamera = nullptr;
    // lights
    std::vector<Light*> Lights;
    // render type
    RenderType renderType;
    // IBL data
    IBLData IBLtextures;
    // renderable members
    std::vector<Renderable*> renderables;
    // object members
    std::vector<GameObject*> gameObjects;
    // scene request functions
    void RequestPush(unsigned int sceneID) { Request = {SceneOp::Push, sceneID}; }
    void RequestPop() { Request = {SceneOp::Pop, {}}; }

private:
    SceneRequest Request;
};