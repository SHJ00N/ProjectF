#include <iostream>

#include "game.h"
#include "resource_manager.h"
#include "game_play_scene.h"

Game::Game(unsigned int width, unsigned int height) : Width(width), Height(height)
{
}

Game::~Game()
{

}

void Game::Init()
{
    m_sceneStack.push_back(createScene(0));
    m_sceneStack.back()->Init();
}

void Game::Update(float dt)
{
    m_sceneStack.back()->Update(dt);
}

void Game::ProcessInput(float dt)
{
    m_sceneStack.back()->ProcessInput(dt);
}

void Game::Render(float dt)
{
    m_sceneStack.back()->Render(dt);
}

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
