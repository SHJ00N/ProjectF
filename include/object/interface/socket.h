#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

#include "animation/animator.h"
#include "object/transform.h"

class ISocket
{
public:
    virtual ~ISocket() = default;
    
    void InitSocket(Animator *animator, Transform *transform)
    {
        m_animator = animator;
        m_transform = transform;

        SocketConfig();
    }

    virtual void SocketConfig() = 0;

    glm::vec3 GetSocketLocalPosition(const std::string &name)
    {
        if(m_sockets.find(name) == m_sockets.end()) return glm::vec3(0.0f);
        return glm::vec3(getSocketMat(name)[3]);
    }
    glm::vec3 GetSocketGlobalPosition(const std::string &name)
    {
        if(m_sockets.find(name) == m_sockets.end()) return glm::vec3(0.0f);
        glm::mat4 boneGlobalModel = m_transform->GetModelMatrix() * getSocketMat(name);
        return glm::vec3(boneGlobalModel[3]);
    }
    glm::mat4 GetSocketLocalMatrix(const std::string &name)
    {
        if(m_sockets.find(name) == m_sockets.end()) return glm::mat4(1.0f);
        glm::mat4 localMat = getSocketMat(name);
        return localMat;
    }
    glm::mat4 GetSocketGlobalMatrix(const std::string &name)
    {
        if(m_sockets.find(name) == m_sockets.end()) return glm::mat4(1.0f);
        return m_transform->GetModelMatrix() * getSocketMat(name);
    }

protected:
    std::unordered_map<std::string, int> m_sockets;
    Animator *m_animator;
    Transform *m_transform;

    const glm::mat4& getSocketMat(const std::string& name)
    {
        const int soket = m_sockets[name];
        const auto& bones = m_animator->GetGlobalBoneMatrices();
        return bones[soket];
    }
};