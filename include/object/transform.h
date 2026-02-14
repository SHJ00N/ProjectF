#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform
{
public:
    // setter
    void ComputeModelMatrix()
	{
		m_modelMatrix = getLocalModelMatrix();
		m_isDirty = false;
	}
	void ComputeModelMatrix(const glm::mat4& parentGlobalModelMatrix)
	{
		m_modelMatrix = parentGlobalModelMatrix * getLocalModelMatrix();
		m_isDirty = false;
	}
	void SetLocalPosition(const glm::vec3& newPosition)
	{
		m_position = newPosition;
		m_isDirty = true;
	}
	void SetLocalRotation(const glm::vec3& newRotation)
	{
		m_rotation = newRotation;
		m_isDirty = true;
	}

	void SetLocalScale(const glm::vec3& newScale)
	{
		m_scale = newScale;
		m_isDirty = true; 
    }

	void SetModelMatrix(const glm::mat4& newModelMatrix)
	{
		m_modelMatrix = newModelMatrix;
		m_isDirty = false;
	}

    // getter
	const glm::vec3 GetGlobalPosition() const { return m_modelMatrix[3]; }
	const glm::vec3& GetLocalPosition() const { return m_position; }
	const glm::vec3& GetLocalRotation() const { return m_rotation; }
	const glm::vec3& GetLocalScale() const { return m_scale; }
	const glm::mat4 GetLocalMatrix() { return getLocalModelMatrix(); }
	const glm::mat4& GetModelMatrix() const { return m_modelMatrix; }
	glm::vec3 GetRight() const { return m_modelMatrix[0]; }
	glm::vec3 GetUp() const { return m_modelMatrix[1]; }
	glm::vec3 GetBackward() const { return m_modelMatrix[2]; }
	glm::vec3 GetForward() const { return -m_modelMatrix[2]; }
	glm::vec3 GetGlobalScale() const { return { glm::length(GetRight()), glm::length(GetUp()), glm::length(GetBackward()) }; }
	bool IsDirty() const { return m_isDirty; }

protected:
    // local space info
    glm::vec3 m_position = { 0.0f, 0.0f, 0.0f};
    glm::vec3 m_rotation = { 0.0f, 0.0f, 0.0f};
    glm::vec3 m_scale = { 1.0f, 1.0f, 1.0f };
    // global space info
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);
    // flag for current transform state
    bool m_isDirty = true;

    // calculate local model matrix
    glm::mat4 getLocalModelMatrix()
    {
        const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		// Y * X * Z
		const glm::mat4 rotationMatrix = transformY * transformX * transformZ;

		// translation * rotation * scale (also know as TRS matrix)
		return glm::translate(glm::mat4(1.0f), m_position) * rotationMatrix * glm::scale(glm::mat4(1.0f), m_scale);
    };
};