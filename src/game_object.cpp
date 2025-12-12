#include "game_object.h"

GameObject::GameObject() : Position(glm::vec3(0.0f)), Size(glm::vec3(1.0f)), RotationAxis(glm::vec3(0.0f, 1.0f, 0.0f)), RotationAngle(0.0f), Velocity(glm::vec2(0.0f)), Destroyed(false), Model3D(nullptr) { }

GameObject::GameObject(Model &model, glm::vec3 position, glm::vec3 size, glm::vec3 rotationAxis, float rotationAngle, glm::vec2 velocity)
    : Model3D(&model), Position(position), Size(size), RotationAxis(rotationAxis), RotationAngle(rotationAngle), Velocity(velocity), Destroyed(false)
{
}
