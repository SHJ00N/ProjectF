#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "model.h"
#include "camera.h"
#include "chunk.h"

const float PLAYER_SPEED = 10.0f;

class Player : public GameObject
{
public:
    float Speed;
    // constructor(s)
    Player();
    Player(Model &model, glm::vec3 position, glm::vec3 size, glm::vec3 rotation = glm::vec3(0.0f), glm::vec2 velocity = glm::vec2(0.0f));

    // set player y position based on terrain height
    void UpdateHeight(float terrainHeight, float dt);

    // player controls
    void Move(Camera_Movement direction, Camera &camera, Chunk *chunk, float dt);
};