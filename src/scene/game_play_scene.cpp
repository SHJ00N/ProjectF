#include <iostream>

#include "scene/game_play_scene.h"
#include "resource_manager.h"
#include "render/renderer/terrain_renderer.h"
#include "light/direction_light.h"
#include "world/world.h"
#include "object/player.h"
#include "object/bone_demo_obj.h"
#include "object/weapon.h"

GamePlayScene::GamePlayScene(unsigned int width, unsigned int height) : Scene(width, height)
{
    // set render type
    renderType = RenderType::Deferred;
}

GamePlayScene::~GamePlayScene()
{
    delete MainCamera; // parent camera pointer
    // parent lights pointer vector
    for(const auto& iter : Lights)
    {
        delete iter;
    }
    Lights.clear();
    // current scene class pointers
    delete player;
    delete boneDemoObj;
    delete world;
    delete terrainRenderer;
    delete sword;
    // delete IBL textures
    IBLtextures.Destroy();
}

void GamePlayScene::Init()
{
    // load shaders
    ResourceManager::LoadShader("shaders/model_shader/boneMesh.vert", "shaders/model_shader/mesh.frag", nullptr, nullptr, nullptr, "boneModel");
    ResourceManager::LoadShader("shaders/model_shader/staticMesh.vert", "shaders/model_shader/mesh.frag", nullptr, nullptr, nullptr, "staticModel");
    ResourceManager::LoadShader("shaders/terrain_shader/terrain.vert", "shaders/terrain_shader/terrain.frag", nullptr, "shaders/terrain_shader/terrain.tcs", "shaders/terrain_shader/terrain.tes", "terrainShader");
    ResourceManager::LoadShader("shaders/csm_shader/terrainShadow.vert", "shaders/csm_shader/cascadedShadow.frag", "shaders/csm_shader/cascadedShadow.geom", "shaders/csm_shader/terrainShadow.tesc", "shaders/csm_shader/terrainShadow.tese", "terrainShadow");
    ResourceManager::LoadShader("shaders/PBR/background.vert", "shaders/PBR/background.frag", nullptr, nullptr, nullptr, "background");
    // configure shaders
    ResourceManager::GetShader("background").Use();
    ResourceManager::GetShader("background").SetInteger("environmentMap", 0);

    // load terrain texture
    ResourceManager::LoadTerrainTexture("resources/texture/Diffuse_16BIT_PNG.png", "resources/texture/CombinedNormal_8BIT_PNG.png", "resources/texture/Roughness_16BIT_PNG.png", "snowField", true);
    // create world and renderer
    world = new World("resources/texture/Heightmap_16BIT_PNG.png", 1.0f, 640.0f, 4, 64.0f, 40);
    terrainRenderer = new TerrainRenderer(ResourceManager::GetShader("terrainShader"), ResourceManager::GetShader("terrainShadow"), *world, ResourceManager::GetTerrainTexture("snowField"));

    // load models
    ResourceManager::LoadModel("resources/object/knight2/SKM_DKM_Full.fbx", true, "knight");
    ResourceManager::LoadModel("resources/object/SM_DKM_Sword.fbx", true, "sword");
    // load animations
    ResourceManager::LoadAnimation("resources/animation/knight2/Anim_DKM_Walk_Alert_Fwd.fbx", ResourceManager::GetModel("knight"), "knight_idle");
    ResourceManager::LoadAnimation("resources/animation/knight2/Anim_DKM_Attack_03.fbx", ResourceManager::GetModel("knight"), "knight_run");
    // create IBL textures
    IBLtextures = IBLGenerator::GenerateIBLFromHDR("resources/texture/galaxy_hdr.png");
    // create main camera
    MainCamera = new Camera();
    // create lights
    Lights.push_back(new DirLight(LightType::Direction, glm::vec3(-0.6f, -1.0f, 0.7f), glm::vec3(0.4f, 0.6f, 1.0f), 0.7f));

    // create game objects and set their default animation
    player = new Player(ResourceManager::GetModel("knight"), ResourceManager::GetShader("boneModel"), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f), glm::vec3(0.0f, 90.0f, 0.0f));
    player->SetAnimation(&ResourceManager::GetAnimation("knight_idle"));
    renderables.push_back(player); // add to renderables list
    gameObjects.push_back(player); // add to game objects list

    sword = new Weapon(ResourceManager::GetModel("sword"), ResourceManager::GetShader("staticModel"), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(1.0f));
    player->AttachWeapon(sword);
    renderables.push_back(sword);

    boneDemoObj = new BoneDemoObj(ResourceManager::GetModel("knight"), ResourceManager::GetShader("staticModel"), glm::vec3(0.0f), glm::vec3(0.01f), glm::vec3(270.0f, 90.0f, 0.0f));
    renderables.push_back(boneDemoObj); // add to renderables list
    gameObjects.push_back(boneDemoObj); // add to game objects list

    Start();
}

void GamePlayScene::Start()
{
    glm::vec3 playerWorldPos = player->transform.GetGlobalPosition();
    player->SetWorldHeight(world->GetWorldHeight(playerWorldPos.x, playerWorldPos.z));
    glm::vec3 demoWorldPos = boneDemoObj->transform.GetGlobalPosition();
    boneDemoObj->SetWorldHeight(world->GetWorldHeight(demoWorldPos.x, demoWorldPos.z));
}

void GamePlayScene::Update(float dt)
{
    // update world
    world->Update(MainCamera->cameraPos);
    // update all game objects
    for(auto& object : gameObjects)
    {
        object->Update(dt);
    }
    // update camera
    MainCamera->Update(player->transform.GetGlobalPosition(), dt);
    glm::mat4 projection = glm::perspective(glm::radians(MainCamera->fov), (float)Width / (float)Height, MainCamera->nearPlane, MainCamera->farPlane);
    glm::mat4 view = MainCamera->GetViewMatrix();
    ResourceManager::GetShader("terrainShader").Use().SetMatrix4("projection", projection);
    ResourceManager::GetShader("terrainShader").SetMatrix4("view", view);
}

void GamePlayScene::ProcessInput(float dt)
{
    if(Keys[GLFW_KEY_W])
    {
        player->Move(FORWARD, *MainCamera, *world, dt);
        player->SetAnimation(&ResourceManager::GetAnimation("knight_run"));
    } 
    if(Keys[GLFW_KEY_S])
    {
        player->Move(BACKWARD, *MainCamera, *world, dt);
        player->SetAnimation(&ResourceManager::GetAnimation("knight_run"));
    }
    if (Keys[GLFW_KEY_A])
    {
        player->Move(LEFT, *MainCamera, *world, dt);
        player->SetAnimation(&ResourceManager::GetAnimation("knight_run"));
    }   
    if (Keys[GLFW_KEY_D])
    {
        player->Move(RIGHT, *MainCamera, *world, dt);
        player->SetAnimation(&ResourceManager::GetAnimation("knight_run"));
    }
    if(!Keys[GLFW_KEY_W] && !Keys[GLFW_KEY_S] && !Keys[GLFW_KEY_A] && !Keys[GLFW_KEY_D])
    {
        player->SetAnimation(&ResourceManager::GetAnimation("knight_idle"));
    }
}

void GamePlayScene::End()
{

}