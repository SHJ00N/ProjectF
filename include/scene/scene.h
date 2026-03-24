#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <memory>
#include <string>

#include "camera.h"
#include "ibl_generator.h"
#include "object/entity.h"
#include "object/game_object.h"
#include "object/interface/renderable.h"
#include "object/interface/collidable.h"

class TerrainRenderer;
class Light;
class Collidable;

// operator for scene management
enum class SceneOp { None, Push, Pop };
struct SceneRequest
{
    SceneOp op = SceneOp::None;
    unsigned int sceneID;
};

enum class RenderType { Forward, Deferred };

struct UIText
{
    std::string text;
    float x, y;
    float scale;
    glm::vec3 color;
};

enum class SceneState
{
    Running,
    End
};

class Scene
{
public:
    // key processing members
    bool Keys[1024];
    bool KeysProcessed[1024];
    // mouse button processing members
    bool MouseButtonLeft;
    bool MouseButtonRight;
    
    // scene graph
    std::unique_ptr<Entity> Root;

    // constructor(s)
    Scene(){ };
    Scene(unsigned int width, unsigned int height) : Width(width), Height(height), Keys{0}, KeysProcessed{0}, MouseButtonLeft(false), MouseButtonRight(false) { };
    virtual ~Scene() = default;

    // init scene
    virtual void Init() = 0;
    // scene processing functions
    virtual void Start() = 0;
    virtual void Update(float dt) = 0;
    virtual void ProcessInput(float dt) = 0;
    virtual void End() = 0;
    virtual void UIUpdate() = 0;
    void CleanUpLists() 
    {
        gameObjects.erase(std::remove_if(gameObjects.begin(), gameObjects.end(), [](GameObject *obj){ return obj->EntityDestroyed; }), gameObjects.end());
        renderables.erase(std::remove_if(renderables.begin(), renderables.end(), [](Renderable *obj){ return obj->RenderableDestroyed; }), renderables.end());
        collidables.erase(std::remove_if(collidables.begin(), collidables.end(), [](Collidable *obj){ return obj->CollidableDestroyed; }), collidables.end());
    }
    
    // getter
    SceneRequest GetSceneRequest() { return Request; }
    Camera* GetCamera() { return MainCamera; }
    std::vector<Light*> GetLights() { return Lights; }
    RenderType GetRenderType() { return renderType; }
    IBLData GetIBLData() { return IBLtextures; }
    std::vector<GameObject*>& GetGameObjects() { return gameObjects; }
    std::vector<Renderable*>& GetRenderables() { return renderables; }
    std::vector<Collidable*>& GetCollidables() { return collidables; }
    std::vector<UIText>& GetUITexts() { return uiTexts; }
    TerrainRenderer* GetTerrainRenderer() { return terrainRenderer; }
    SceneState GetSceneState() { return sceneState; }
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
    // collidable members for debug
    std::vector<Collidable*> collidables;
    // text members
    std::vector<UIText> uiTexts;
    // terrain renderer
    TerrainRenderer *terrainRenderer = nullptr;
    // scene request functions
    void RequestPush(unsigned int sceneID) { Request = {SceneOp::Push, sceneID}; }
    void RequestPop() { Request = {SceneOp::Pop, {}}; }

    SceneState sceneState = SceneState::Running;

private:
    SceneRequest Request;
};