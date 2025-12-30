#include <iostream>

#include "game_play_scene.h"
#include "resource_manager.h"
#include "chunk_grid.h"
#include "skeletal_mesh_renderer.h"
#include "player.h"

SkeletalMeshRenderer *skeletalRenderer;
ChunkGrid *world;
Player *player;

GamePlayScene::GamePlayScene(unsigned int width, unsigned int height) : Scene(width, height)
{
}

GamePlayScene::~GamePlayScene()
{
    delete MainCamera;
    delete skeletalRenderer;
    delete world;
    delete player;
}

void GamePlayScene::Init()
{
    // load shaders
    ResourceManager::LoadShader("shaders/sample.vert", "shaders/sample.frag", nullptr, nullptr, nullptr, "modelLoading");
    ResourceManager::LoadShader("shaders/gpuheight.vert", "shaders/gpuheight.frag", nullptr, "shaders/gpuheight.tcs", "shaders/gpuheight.tes", "terrainShader");
    // load models
    ResourceManager::LoadModel("resources/object/knight/knight.dae", false, "knight");
    // load animations
    ResourceManager::LoadAnimation("resources/animation/knight/knight_idle.dae", ResourceManager::GetModel("knight"), "knight_idle");
    ResourceManager::LoadAnimation("resources/animation/knight/knight_run.dae", ResourceManager::GetModel("knight"), "knight_run");
    // load terrains
    ResourceManager::LoadTerrain("resources/texture/Diffuse_16BIT_PNG.png", "resources/texture/Heightmap_16BIT_PNG.png", "resources/texture/CombinedNormal_8BIT_PNG.png", "snowField", 1024.0f, 1.0f, 64);

    // create main camera
    MainCamera = new Camera();

    // create renderers
    skeletalRenderer = new SkeletalMeshRenderer();

    // create chunk grid and create chunks
    world = new ChunkGrid(2);
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
    player = new Player(ResourceManager::GetModel("knight"), glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f, 90.0f, 0.0f));
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
    ResourceManager::GetShader("modelLoading").Use().SetMatrix4("projection", projection);
    ResourceManager::GetShader("modelLoading").SetMatrix4("view", view);
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
    skeletalRenderer->Draw(ResourceManager::GetShader("modelLoading"), *player, dt);
    world->Draw(ResourceManager::GetShader("terrainShader"));
}

void GamePlayScene::End()
{

}
