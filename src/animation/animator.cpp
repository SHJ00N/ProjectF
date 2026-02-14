#include "animation/animator.h"
#include "animation/animation.h"

#include <iostream>

Animator::Animator(){
    m_CurrentTime = 0.0;
    m_CurrentAnimation = nullptr;

    m_FinalBoneMatrices.reserve(100);
    m_GlobalBoneMatrices.reserve(100);

    for(int i = 0; i < 100; i++)
    {
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
        m_GlobalBoneMatrices.push_back(glm::mat4(1.0f));
    }
}

void Animator::UpdateAnimation(float dt){
    if(!m_CurrentAnimation) return;

    // update current animation time
    m_DeltaTime = dt;
    m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
    m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());

    // if request animation change, animation blending perform
    if(m_isBlending)
    {
        // update next animation time
        m_nextTime += m_nextAnimation->GetTicksPerSecond() * dt;
        m_nextTime = fmod(m_nextTime, m_nextAnimation->GetDuration());
        // calculate blend factor
        m_blendTime += dt;
        float weight = glm::clamp(m_blendTime / m_blendDuration, 0.0f, 1.0f);

        // end blending
        if(weight >= 1.0f)
        {
            m_CurrentAnimation = m_nextAnimation;
            m_CurrentTime = m_nextTime;
            m_nextAnimation = nullptr;
            m_isBlending = false;

            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
            return;
        }

        // blending
        CalculateBoneTransformBlended(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f), weight);
    }
    else
    {
        CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
    }
}

void Animator::PlayAnimation(Animation *pAnimation){
    // first call
    if(!m_CurrentAnimation)
    {
        m_CurrentAnimation = pAnimation;
        m_CurrentTime = 0.0f;
        return;
    }
    // set blending member
    if(m_CurrentAnimation != pAnimation && m_nextAnimation != pAnimation)
    {
        m_nextAnimation = pAnimation;
        m_nextTime = 0.0f;
        m_blendTime = 0.0f;
        m_isBlending = true;
    }
}

void Animator::CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform){
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    Bone *Bone = m_CurrentAnimation->FindBone(nodeName);

    if(Bone){
        Bone->Update(m_CurrentTime);
        nodeTransform = Bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
    if(boneInfoMap.find(nodeName) != boneInfoMap.end()){
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        m_FinalBoneMatrices[index] = globalTransformation * offset;

        // for soket
        m_GlobalBoneMatrices[index] = globalTransformation;
    }

    for(int i = 0; i < node->childrenCount; i++)
        CalculateBoneTransform(&node->children[i], globalTransformation);
}

void Animator::CalculateBoneTransformBlended(const AssimpNodeData *node, glm::mat4 parentTransform, float weight)
{
    std::string nodeName = node->name;
    // 루트 노드 이름 : RootNode(root motion 이동 제거 할 때 필요)
    glm::mat4 nodeTransform = node->transformation;
    // bone data
    Bone *curBone = m_CurrentAnimation->FindBone(nodeName);
    Bone *nextBone = m_nextAnimation->FindBone(nodeName);
    if(curBone) curBone->Update(m_CurrentTime);
    if(nextBone) nextBone->Update(m_nextTime);

    if(curBone && nextBone)
    {
        // blended local transform
        glm::vec3 position = glm::mix(curBone->GetLocalPosition(), nextBone->GetLocalPosition(), weight);
        glm::quat rotation = glm::slerp(curBone->GetLocalRotation(), nextBone->GetLocalRotation(), weight);
        glm::vec3 scale = glm::mix(curBone->GetLocalScale(), nextBone->GetLocalScale(), weight);

        if(nodeName == "RootNode")
        {
            position.x = 0.0f;
            position.z = 0.0f;
        }

        glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 R = glm::toMat4(rotation);
        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
        nodeTransform = T * R * S;
    } 
    else if(curBone)
    {
        nodeTransform = curBone->GetLocalTransform();
    }
    else if(nextBone)
    {
        nodeTransform = nextBone->GetLocalTransform();
    }

    glm::mat4 globalBlendedTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
    if(boneInfoMap.find(nodeName) != boneInfoMap.end()){
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        m_FinalBoneMatrices[index] = globalBlendedTransformation * offset;

        // for soket
        m_GlobalBoneMatrices[index] = globalBlendedTransformation;
    }

    for(int i = 0; i < node->childrenCount; i++)
        CalculateBoneTransformBlended(&node->children[i], globalBlendedTransformation, weight);
}

std::vector<glm::mat4> Animator::GetFinalBoneMatrices(){
    return m_FinalBoneMatrices;
}

std::vector<glm::mat4> Animator::GetGlobalBoneMatrices(){
    return m_GlobalBoneMatrices;
}