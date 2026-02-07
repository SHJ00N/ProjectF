#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

class Animation;
class AssimpNodeData;

class Animator{
public:
    Animator();
    void UpdateAnimation(float dt);
    void PlayAnimation(Animation *pAnimation);
    void CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform);
    std::vector<glm::mat4> GetFinalBoneMatrices();

private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    Animation *m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;
};