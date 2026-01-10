#include <iostream>

#include "game_play_scene.h"
#include "resource_manager.h"

#include "direction_light.h"

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
    delete skeletalRenderer;
    delete world;
    delete player;
    // delete IBL textures
    IBLtextures.Destroy();
}

void GamePlayScene::Init()
{
    // load shaders
    ResourceManager::LoadShader("shaders/model_shader/boneMesh.vert", "shaders/model_shader/mesh.frag", nullptr, nullptr, nullptr, "boneModel");
    ResourceManager::LoadShader("shaders/terrain_shader/terrain.vert", "shaders/terrain_shader/terrain.frag", nullptr, "shaders/terrain_shader/terrain.tcs", "shaders/terrain_shader/terrain.tes", "terrainShader");
    ResourceManager::LoadShader("shaders/PBR/background.vert", "shaders/PBR/background.frag", nullptr, nullptr, nullptr, "background");
    // configure shaders
    ResourceManager::GetShader("background").Use();
    ResourceManager::GetShader("background").SetInteger("environmentMap", 0);

    // load models
    ResourceManager::LoadModel("resources/object/knight2/SKM_DKM_Full_With_Sword.fbx", true, "knight");
    // load animations
    ResourceManager::LoadAnimation("resources/animation/knight2/Anim_DKM_Idle.fbx", ResourceManager::GetModel("knight"), "knight_idle");
    ResourceManager::LoadAnimation("resources/animation/knight2/Anim_DKM_Run_Fwd.fbx", ResourceManager::GetModel("knight"), "knight_run");
    // load terrains
    ResourceManager::LoadTerrain("resources/texture/Diffuse_16BIT_PNG.png", "resources/texture/Heightmap_16BIT_PNG.png", "resources/texture/CombinedNormal_8BIT_PNG.png", "resources/texture/Roughness_16BIT_PNG.png", "snowField", 1024.0f, 2.0f, 20);
    // create IBL textures
    IBLtextures = IBLGenerator::GenerateIBLFromHDR("resources/texture/galaxy_hdr.png");
    // create main camera
    MainCamera = new Camera();
    // create lights
    Lights.push_back(new DirLight(LightType::Direction, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.7f, 0.8f, 1.0f), 1.0f));
    // create renderers
    skeletalRenderer = new SkeletalMeshRenderer();
    // create chunk grid and create chunks
    world = new ChunkGrid(1);
    int n = world->GetGridSize();
    for(int z = 0; z < n; z++)
    {
        for(int x = 0; x < n ; x++)
        {
            world->SetupChunkTerrain(x, z, ResourceManager::GetTerrain("snowField"));
            world->GetChunk(x, z)->ChunkTransform.scale = glm::vec3(x % 2 == 0 ? 1.0f : -1.0f, 1.0f, z % 2 == 0 ? -1.0f : 1.0f);
        }
    }

    // create game objects and set their default animation
    player = new Player(ResourceManager::GetModel("knight"), glm::vec3(0.0f), glm::vec3(0.01f), glm::vec3(0.0f, 90.0f, 0.0f));
    player->Animator3D.PlayAnimation(&ResourceManager::GetAnimation("knight_idle"));
    player->UpdateHeight(world->GetChunk(player->ObjectTransform.position.x, player->ObjectTransform.position.z)->GetWorldHeight(player->ObjectTransform.position.x, player->ObjectTransform.position.z), 0);

    Start();
}

void GamePlayScene::Start()
{

}

void GamePlayScene::Update(float dt)
{
    MainCamera->Update(player->ObjectTransform.position, dt);
    glm::mat4 projection = glm::perspective(glm::radians(MainCamera->fov), (float)Width / (float)Height, 0.1f, 5000.0f);
    glm::mat4 view = MainCamera->GetViewMatrix();
    ResourceManager::GetShader("terrainShader").Use().SetMatrix4("projection", projection);
    ResourceManager::GetShader("terrainShader").SetMatrix4("view", view);
}

void GamePlayScene::ProcessInput(float dt)
{
    Chunk *curPlayerChunk = world->GetChunk(player->ObjectTransform.position.x, player->ObjectTransform.position.z);
    if(Keys[GLFW_KEY_W])
    {
        player->Move(FORWARD, *MainCamera, curPlayerChunk, dt);
        player->Animator3D.PlayAnimation(&ResourceManager::GetAnimation("knight_run"));
    } 
    if(Keys[GLFW_KEY_S])
    {
        player->Move(BACKWARD, *MainCamera, curPlayerChunk, dt);
        player->Animator3D.PlayAnimation(&ResourceManager::GetAnimation("knight_run"));
    }
    if (Keys[GLFW_KEY_A])
    {
        player->Move(LEFT, *MainCamera, curPlayerChunk, dt);
        player->Animator3D.PlayAnimation(&ResourceManager::GetAnimation("knight_run"));
    }   
    if (Keys[GLFW_KEY_D])
    {
        player->Move(RIGHT, *MainCamera, curPlayerChunk, dt);
        player->Animator3D.PlayAnimation(&ResourceManager::GetAnimation("knight_run"));
    }
    if(!Keys[GLFW_KEY_W] && !Keys[GLFW_KEY_S] && !Keys[GLFW_KEY_A] && !Keys[GLFW_KEY_D])
    {
        player->Animator3D.PlayAnimation(&ResourceManager::GetAnimation("knight_idle"));
    }
}

void GamePlayScene::Render(float dt)
{
    skeletalRenderer->Draw(ResourceManager::GetShader("boneModel"), *player, dt);
    world->Draw(ResourceManager::GetShader("terrainShader"));
}

void GamePlayScene::End()
{

}