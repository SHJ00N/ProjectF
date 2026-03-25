#include <iostream>
#include <memory>

#include "scene/game_play_scene.h"
#include "resource_manager.h"
#include "render/renderer/terrain_renderer.h"
#include "light/direction_light.h"
#include "world/world.h"
#include "object/player.h"
#include "object/weapon.h"
#include "object/game_object.h"
#include "particle/particle_manager.h"
#include "collision_system.h"
#include "particle/blood_particle.h"
#include "object/enemy/enemy.h"
#include "object/enemy/enemy_spawn_manager.h"
#include "particle/particle_manager.h"
#include "sound_manager.h"

std::vector<glm::vec3> wayPoints
{
    glm::vec3(2046.0f, 0.0f, 2046.0f),
    glm::vec3(2050.0f, 0.0f, 2050.0f)
};

GamePlayScene::GamePlayScene(unsigned int width, unsigned int height) : Scene(width, height)
{
    // set render type
    renderType = RenderType::Deferred;

    Root = std::make_unique<Entity>();
}

GamePlayScene::~GamePlayScene()
{
    // all entity destroy
    Root.reset();

    delete MainCamera; // parent camera pointer
    // parent lights pointer vector
    for(const auto& iter : Lights)
    {
        delete iter;
    }
    Lights.clear();
    // current scene class pointers
    delete world;
    delete terrainRenderer;
    delete collisionSystem;
    delete enemySpawnManager;
    // all lists clear
    gameObjects.clear();
    collidables.clear();
    renderables.clear();
    // delete IBL textures
    IBLtextures.Destroy();
}

void GamePlayScene::Init()
{
    collisionSystem = new CollisionSystem();
    
    enemySpawnManager = new EnemySpawnManager();
    enemySpawnManager->LoadSpawnDataFromFile("resources/text/spawn_data.txt");

    // load sounds
    SoundManager::GetInstance().Init();
    SoundManager::GetInstance().LoadSound("resources/sound/Footstep_Sand_05.wav", "footStep", true);
    SoundManager::GetInstance().LoadSound("resources/sound/Hit_Metal.wav", "hit");
    SoundManager::GetInstance().LoadSound("resources/sound/slash.wav", "slash");
    SoundManager::GetInstance().LoadSound("resources/sound/wind.mp3", "wind", true);

    // load shaders
    ResourceManager::LoadShader("shaders/model_shader/boneMesh.vert", "shaders/model_shader/mesh.frag", nullptr, nullptr, nullptr, "boneModel");
    ResourceManager::LoadShader("shaders/model_shader/staticMesh.vert", "shaders/model_shader/mesh.frag", nullptr, nullptr, nullptr, "staticModel");
    ResourceManager::LoadShader("shaders/terrain_shader/terrain.vert", "shaders/terrain_shader/terrain.frag", nullptr, "shaders/terrain_shader/terrain.tcs", "shaders/terrain_shader/terrain.tes", "terrainShader");
    ResourceManager::LoadShader("shaders/csm_shader/terrainShadow.vert", "shaders/csm_shader/cascadedShadow.frag", "shaders/csm_shader/cascadedShadow.geom", "shaders/csm_shader/terrainShadow.tesc", "shaders/csm_shader/terrainShadow.tese", "terrainShadow");
    ResourceManager::LoadShader("shaders/particle_shader/blood.vert", "shaders/particle_shader/blood.frag", nullptr, nullptr, nullptr, "blood");
    // load compute shaders
    ResourceManager::LoadComputeShader("shaders/particle_shader/compute.comp", "bloodCompute");

    // load texture
    ResourceManager::LoadTerrainTexture("resources/texture/Diffuse_16BIT_PNG.png", "resources/texture/CombinedNormal_8BIT_PNG.png", "resources/texture/Roughness_16BIT_PNG.png", "snowField", false);
    ResourceManager::LoadTexture("resources/texture/blood1.png", false, "blood_splatter", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    // create world and renderer
    world = new World("resources/texture/Heightmap_16BIT_PNG.png", 1.0f, 640.0f, 4, 64.0f, 40, 2);
    terrainRenderer = new TerrainRenderer(ResourceManager::GetShader("terrainShader"), ResourceManager::GetShader("terrainShadow"), *world, ResourceManager::GetTerrainTexture("snowField"));
    
    // load models
    ResourceManager::LoadModel("resources/object/knight2/SKM_DKM_Full.fbx", false, "knight");
    ResourceManager::LoadModel("resources/object/weapon/SM_DKM_Sword.fbx", false, "sword");
    ResourceManager::LoadModel("resources/object/Enemy/SKM_DKF_Full.fbx", true, "enemy");
    ResourceManager::LoadModel("resources/object/weapon/SM_DKM_Sword.fbx", true, "enemy_sword");
    // load animations
    // player
    ResourceManager::LoadAnimation("resources/animation/knight2/Anim_DKM_Idle_Alert.fbx", ResourceManager::GetModel("knight"), "knight_idle");
    ResourceManager::LoadAnimation("resources/animation/knight2/Anim_DKM_Walk_Alert_Fwd.fbx", ResourceManager::GetModel("knight"), "knight_walk");
    ResourceManager::LoadAnimation("resources/animation/knight2/Anim_DKM_Run_Alert_Fwd.fbx", ResourceManager::GetModel("knight"), "knight_run");
    ResourceManager::LoadAnimation("resources/animation/knight2/Anim_DKM_Attack_01.fbx", ResourceManager::GetModel("knight"), "knight_attack1", false);
    ResourceManager::LoadAnimation("resources/animation/knight2/Stand To Roll.fbx", ResourceManager::GetModel("knight"), "knight_roll", false);
    ResourceManager::LoadAnimation("resources/animation/knight2/Anim_DKM_Hit_Alert_Fwd.fbx", ResourceManager::GetModel("knight"), "knight_hit", false);
    ResourceManager::LoadAnimation("resources/animation/knight2/Anim_DKM_Death.fbx", ResourceManager::GetModel("knight"), "knight_death", false);
    // enemy
    ResourceManager::LoadAnimation("resources/animation/enemy/Anim_DKF_Idle_Alert.fbx", ResourceManager::GetModel("enemy"), "enemy_idle");
    ResourceManager::LoadAnimation("resources/animation/enemy/Anim_DKF_Walk_Alert_Fwd.fbx", ResourceManager::GetModel("enemy"), "enemy_walk");
    ResourceManager::LoadAnimation("resources/animation/enemy/Anim_DKF_Attack_01.fbx", ResourceManager::GetModel("enemy"), "enemy_attack1", false);
    ResourceManager::LoadAnimation("resources/animation/enemy/Anim_DKF_Hit_Alert_Fwd.fbx", ResourceManager::GetModel("enemy"), "enemy_hit", false);
    ResourceManager::LoadAnimation("resources/animation/enemy/Anim_DKF_Death.fbx", ResourceManager::GetModel("enemy"), "enemy_death", false);
    ResourceManager::LoadAnimation("resources/animation/enemy/Anim_DKF_Run_Alert_Fwd.fbx", ResourceManager::GetModel("enemy"), "enemy_run");
    // create IBL textures
    IBLtextures = IBLGenerator::GenerateIBLFromHDR("resources/texture/galaxy_hdr.png");
    // create main camera
    MainCamera = new Camera();
    // create lights
    Lights.push_back(new DirLight(LightType::Direction, glm::vec3(-0.6f, -1.0f, 0.7f), glm::vec3(0.4f, 0.6f, 1.0f), 1.0f));

    // create game objects and set animator
    player = &Root->addChild<Player>(ResourceManager::GetModel("knight"), ResourceManager::GetShader("boneModel"), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.01f), glm::vec3(0.0f), glm::vec2(0.0f), Layer::Player);
    player->AddAnimation("Idle", &ResourceManager::GetAnimation("knight_idle"));
    player->AddAnimation("Walk", &ResourceManager::GetAnimation("knight_walk"));
    player->AddAnimation("Run", &ResourceManager::GetAnimation("knight_run"));
    player->AddAnimation("Attack1", &ResourceManager::GetAnimation("knight_attack1"));
    player->AddAnimation("Roll", &ResourceManager::GetAnimation("knight_roll"));
    player->AddAnimation("Hit", &ResourceManager::GetAnimation("knight_hit"));
    player->AddAnimation("Death", &ResourceManager::GetAnimation("knight_death"));
    renderables.push_back(player); // add to renderables list
    gameObjects.push_back(player); // add to game objects list
    collidables.push_back(player);

    // create weapon entity on player
    auto &sword = player->addChild<Weapon>(ResourceManager::GetModel("sword"), ResourceManager::GetShader("staticModel"), glm::vec3(0.0f), glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(1.0f), Layer::Weapon);
    sword.SetOwner(player);
    collidables.push_back(&sword);
    player->EquipWeapon(&sword);

    Start();
}

void GamePlayScene::Start()
{
    for(auto &object : gameObjects)
    {
        object->Init();
    }
    enemySpawnManager->Init(this, *player);
    player->transform.SetLocalPosition(glm::vec3(2090.0f, 0.0f, 1800.0f));
    player->transform.SetLocalRotation(glm::vec3(0.0f));

    SoundManager::GetInstance().PlayLoop("wind", 1.0f, 0.3f);
}

void GamePlayScene::Update(float dt)
{
    // clear texts
    uiTexts.clear();

    // update all game objects
    ObjectUpdateContext context{dt, world, MainCamera};
    for(auto& object : gameObjects)
    {
        object->Update(context);
    }

    // update particle
    ParticleManager::GetInstance().Update(dt);

    // update camera
    MainCamera->Update(player->GetSocketGlobalPosition("Center"), dt);

    // update world
    world->Update(MainCamera->cameraPos);

    collisionSystem->Step(*world);

    enemySpawnManager->Update(this, *player, dt);

    UIUpdate();
    if(player->Health <= 0) sceneState = SceneState::End;

    // remove destroyed objects
    CleanUpLists();
    Root->cleanupDestroyed();

    // update sound
    SoundManager::GetInstance().Update();
}

void GamePlayScene::ProcessInput(float dt)
{
    // attack input
    if(MouseButtonLeft)
    {
        player->RequestAttack();
    }
    // roll input
    if(Keys[GLFW_KEY_SPACE])
    {
        player->RequestRoll();
    }
    // movement input
    player->RequestMove(glm::vec3(Keys[GLFW_KEY_D] - Keys[GLFW_KEY_A], 0.0f, Keys[GLFW_KEY_W] - Keys[GLFW_KEY_S]), Keys[GLFW_KEY_LEFT_SHIFT]);

    if(sceneState == SceneState::End && Keys[GLFW_KEY_ENTER])
    {
        Start();
        sceneState = SceneState::Running;
    }
}

void GamePlayScene::UIUpdate()
{
    // player Health UI
    uiTexts.push_back(
        {
            "HP : " + std::to_string(player->Health >= 0 ? player->Health : 0),
            20.0f, 20.0f,
            1.0f,
            glm::vec3(1.0f, 1.0f, 1.0f)
        }
    );

    // debug area
    // glm::vec3 playerPos = player->transform.GetGlobalPosition();
    // uiTexts.push_back(
    //     {
    //         "X : " + std::to_string(playerPos.x) + " Y : " + std::to_string(playerPos.y) + " Z : " + std::to_string(playerPos.z),
    //         20.0f, 45.0f,
    //         0.7f,
    //         glm::vec3(1.0f, 1.0f, 1.0f)
    //     }
    // );
    // uiTexts.push_back(
    //     {
    //         "fov : " + std::to_string(MainCamera->fov),
    //         20.0f, 70.0f,
    //         0.7f,
    //         glm::vec3(1.0f, 1.0f, 1.0f)
    //     }
    // );
    // uiTexts.push_back(
    //     {
    //         "near dis : " + std::to_string(MainCamera->frustum.nearFace.distance),
    //         20.0f, 95.0f,
    //         0.7f,
    //         glm::vec3(1.0f, 1.0f, 1.0f)
    //     }
    // );
    // uiTexts.push_back(
    //     {
    //         "far dis : " + std::to_string(MainCamera->frustum.farFace.distance),
    //         20.0f, 120.0f,
    //         0.7f,
    //         glm::vec3(1.0f, 1.0f, 1.0f)
    //     }
    // );
    // uiTexts.push_back(
    //     {
    //         "top dis : " + std::to_string(MainCamera->frustum.topFace.distance),
    //         20.0f, 145.0f,
    //         0.7f,
    //         glm::vec3(1.0f, 1.0f, 1.0f)
    //     }
    // );
    // uiTexts.push_back(
    //     {
    //         "bottom dis : " + std::to_string(MainCamera->frustum.bottomFace.distance),
    //         20.0f, 170.0f,
    //         0.7f,
    //         glm::vec3(1.0f, 1.0f, 1.0f)
    //     }
    // );
    // uiTexts.push_back(
    //     {
    //         "right dis : " + std::to_string(MainCamera->frustum.rightFace.distance),
    //         20.0f, 195.0f,
    //         0.7f,
    //         glm::vec3(1.0f, 1.0f, 1.0f)
    //     }
    // );
    // uiTexts.push_back(
    //     {
    //         "left dis : " + std::to_string(MainCamera->frustum.leftFace.distance),
    //         20.0f, 220.0f,
    //         0.7f,
    //         glm::vec3(1.0f, 1.0f, 1.0f)
    //     }
    // );

    if(sceneState == SceneState::End)
    {
        uiTexts.push_back(
            {
                "Press Enter to restart",
                (float)Width * 0.35f, (float)Height * 0.75f,
                2.0f,
                glm::vec3(1.0f, 1.0f, 1.0f)
            }
        );
    }
}

void GamePlayScene::End()
{
    SoundManager::GetInstance().StopLoop("wind");
}