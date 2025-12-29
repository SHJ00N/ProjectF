#include "game_object.h"

GameObject::GameObject() : Velocity(glm::vec2(0.0f)), Destroyed(false), Model3D(nullptr) { }

GameObject::GameObject(Model &model, glm::vec3 position, glm::vec3 size, glm::vec3 rotation, glm::vec2 velocity) : Model3D(&model), Velocity(velocity), Destroyed(false) 
{
    ObjectTransform.position = position;
    ObjectTransform.scale = size;
    ObjectTransform.rotation = rotation;
}
