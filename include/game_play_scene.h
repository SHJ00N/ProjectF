#pragma once

#include <vector>
#include <string>

#include "scene.h"
#include "camera.h"
#include "chunk_grid.h"
#include "skeletal_mesh_renderer.h"
#include "player.h"
#include "bone_demo_obj.h"

class GamePlayScene : public Scene
{
public: 
    // constructor(s)
    GamePlayScene(unsigned int width, unsigned int height);
    ~GamePlayScene() override;

    // scene's memberfunction overriding
    void Init() override;
    void Start() override;
    void Update(float dt) override;
    void ProcessInput(float dt) override;
    void End() override;

private:
    // obj
    SkeletalMeshRenderer *skeletalRenderer;
    ChunkGrid *world;
    Player *player;
    BoneDemoObj *boneDemoObj;
};