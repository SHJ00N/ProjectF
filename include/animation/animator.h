#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

class Animation;
class AssimpNodeData;

const float BLEND_DURATION = 0.15f;

class Animator{
public:
    Animator();
    void UpdateAnimation(float dt);
    void PlayAnimation(Animation *pAnimation);
    void CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform);
    void CalculateBoneTransformBlended(const AssimpNodeData *node, glm::mat4 parentTransform, float weight);
    std::vector<glm::mat4> GetFinalBoneMatrices();
    std::vector<glm::mat4> GetGlobalBoneMatrices();
private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    // for soket
    std::vector<glm::mat4> m_GlobalBoneMatrices;
    Animation *m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;

    // blend member
    Animation *m_nextAnimation;
    float m_nextTime;
    float m_blendTime;
    float m_blendDuration = BLEND_DURATION;
    bool m_isBlending = false;
};