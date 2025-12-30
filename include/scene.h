#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera.h"

// operator for scene management
enum class SceneOp { None, Push, Pop };
struct SceneRequest
{
    SceneOp op = SceneOp::None;
    unsigned int sceneID;
};

class Scene
{
public:
    // key processing members
    bool Keys[1024];
    bool KeysProcessed[1024];

    // constructor(s)
    Scene(){ };
    Scene(unsigned int width, unsigned int height) : Width(width), Height(height) { };
    virtual ~Scene() = default;

    // init scene
    virtual void Init() = 0;
    // scene processing functions
    virtual void Start() = 0;
    virtual void Update(float dt) = 0;
    virtual void ProcessInput(float dt) = 0;
    virtual void Render(float dt) = 0;
    virtual void End() = 0;
    
    // getter
    SceneRequest GetSceneRequest() { return Request; }
    Camera* GetCamera() { return MainCamera; }

    // setter
    void RequestClear() { Request = {SceneOp::None, {}}; }

protected:
    // screen size
    unsigned int Width, Height;
    
    // all scenes except menu scene have a main camera 
    Camera *MainCamera = nullptr;
    
    // scene request functions
    void RequestPush(unsigned int sceneID) { Request = {SceneOp::Push, sceneID}; }
    void RequestPop() { Request = {SceneOp::Pop, {}}; }

private:
    SceneRequest Request;
};