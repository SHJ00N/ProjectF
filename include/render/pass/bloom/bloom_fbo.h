#pragma once

#include <glm/glm.hpp>
#include <vector>

struct BloomMip
{
    glm::vec2 size;
    glm::ivec2 intSize;
    unsigned int texture;
};

class BloomFBO
{
public:
	BloomFBO();
	~BloomFBO();
	bool Init(unsigned int windowWidth, unsigned int windowHeight, unsigned int mipChainLength);
	void Destroy();
	void BindForWriting();
	const std::vector<BloomMip>& MipChain() const;

private:
	bool m_Init;
	unsigned int m_FBO;
	std::vector<BloomMip> m_MipChain;
};