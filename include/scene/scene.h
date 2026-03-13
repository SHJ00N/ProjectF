#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <memory>

#include "camera.h"
#include "ibl_generator.h"
#include "object/entity.h"
#include "object/game_object.h"
#include "object/interface/renderable.h"
#include "object/interface/collidable.h"
#include "particle/particle_manager.h"

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
    ParticleManager* GetParticleManager() { return particleManager; }
    RenderType GetRenderType() { return renderType; }
    IBLData GetIBLData() { return IBLtextures; }
    std::vector<Renderable*> GetRenderables() { return renderables; }
    std::vector<Collidable*> GetCollidables() { return collidables; }
    TerrainRenderer* GetTerrainRenderer() { return terrainRenderer; }
    // setter
    void RequestClear() { Request = {SceneOp::None, {}}; }

protected:
    // screen size
    unsigned int Width, Height;
    // all scenes except menu scene have a main camera 
    Camera *MainCamera = nullptr;
    // lights
    std::vector<Light*> Lights;
    // particles
    ParticleManager* particleManager;
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
    // terrain renderer
    TerrainRenderer *terrainRenderer = nullptr;
    // scene request functions
    void RequestPush(unsigned int sceneID) { Request = {SceneOp::Push, sceneID}; }
    void RequestPop() { Request = {SceneOp::Pop, {}}; }

private:
    SceneRequest Request;
};