#include "game.h"
#include "resource_manager.h"
#include "game_play_scene.h"
#include "render_system.h"

RenderSystem *renderSystem;

#pragma region lifecycle
Game::Game(unsigned int width, unsigned int height) : Width(width), Height(height)
{
}

Game::~Game()
{
    delete renderSystem;
}

#pragma endregion

#pragma region loop
void Game::Init()
{
    renderSystem = new RenderSystem(Width, Height);
    renderSystem->Init();

    m_sceneStack.push_back(createScene(0));
    m_sceneStack.back()->Init();
}

void Game::Update(float dt)
{
    processSceneRequest();
    // update current scene
    m_sceneStack.back()->Update(dt);
}

void Game::ProcessInput(float dt)
{
    m_sceneStack.back()->ProcessInput(dt);
}

void Game::Render(float dt)
{
    for(const auto &scene : m_sceneStack)
    {
        renderSystem->BeginFrame(scene.get());
        renderSystem->Render(scene.get(), dt);
    }
}

#pragma endregion

#pragma region scene_stack
Scene* Game::GetCurrentScene()
{
    if(m_sceneStack.empty()) return nullptr;
    return m_sceneStack.back().get();
}

void Game::processSceneRequest()
{
    if(m_sceneStack.empty()) return;

    // current scene request
    auto& request = m_sceneStack.back()->GetSceneRequest();
    // init request
    m_sceneStack.back()->RequestClear();

    // process request
    if(request.op == SceneOp::Push)
    {
        auto newScene = createScene(request.sceneID);
        if(!newScene) return;

        m_sceneStack.push_back(std::move(newScene));
        m_sceneStack.back()->Init();
    }
    if(request.op == SceneOp::Pop)
    {
        m_sceneStack.pop_back();

        if(!m_sceneStack.empty()) m_sceneStack.back()->Start();
    }
}

std::unique_ptr<Scene> Game::createScene(unsigned int sceneID)
{
    // first game playing scene
    if(sceneID == 0) return std::make_unique<GamePlayScene>(Width, Height);
    return nullptr;
}

#pragma endregion