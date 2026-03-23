#include "render/pass/bloom/bloom_pass.h"
#include "resource_manager.h"

BloomPass::BloomPass(unsigned int width, unsigned int height) : m_quadVAO(0), m_quadVBO(0)
{
    // load shader
    ResourceManager::LoadShader("shaders/bloom_shader/down_sample.vert", "shaders/bloom_shader/down_sample.frag", nullptr, nullptr, nullptr, "downSample");
    ResourceManager::LoadShader("shaders/bloom_shader/up_sample.vert", "shaders/bloom_shader/up_sample.frag", nullptr, nullptr, nullptr, "upSample");
    
    // shaders
    m_downSampleShader = &ResourceManager::GetShader("downSample");
    m_downSampleShader->Use();
    m_downSampleShader->SetInteger("srcTexture", 0);
    m_upSampleShader = &ResourceManager::GetShader("upSample");
    m_upSampleShader->Use();
    m_upSampleShader->SetInteger("srcTexture", 0);
    glUseProgram(0);
    
    // set private member
    m_srcViewportSize = glm::ivec2(width, height);
    m_srcViewportSizeFloat = glm::vec2(width, height);
    
    // frame buffer
    bool status = m_fbo.Init(width, height, 5);
    if (!status) {
		std::cerr << "Failed to initialize bloom FBO - cannot create bloom renderer!\n";
	}
}

BloomPass::~BloomPass()
{
    m_fbo.Destroy();
    if(m_quadVBO) glDeleteBuffers(1, &m_quadVBO);
    if(m_quadVAO) glDeleteVertexArrays(1, &m_quadVAO);
}

unsigned int BloomPass::GetBloomTexture()
{
    return m_fbo.MipChain()[0].texture;
}

unsigned int BloomPass::GetBloomMip_i(int index)
{
    const std::vector<BloomMip>& mipChain = m_fbo.MipChain();
	int size = (int)mipChain.size();
	return mipChain[(index > size - 1) ? size - 1 : (index < 0) ? 0 : index].texture;
}

void BloomPass::Render(unsigned int srcTexture, float filterRadius)
{
    m_fbo.BindForWriting();

    this->renderDownSamples(srcTexture);
    this->renderUpSamples(filterRadius);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Restore viewport
	glViewport(0, 0, m_srcViewportSize.x, m_srcViewportSize.y);
}

void BloomPass::renderDownSamples(unsigned int srcTexture)
{
    const std::vector<BloomMip>& mipChain = m_fbo.MipChain();

	m_downSampleShader->Use();
	m_downSampleShader->SetVector2f("srcResolution", m_srcViewportSizeFloat);
	if (m_KarisAverageOnDownsample) {
		m_downSampleShader->SetInteger("mipLevel", 0);
	}

	// Bind srcTexture (HDR color buffer) as initial texture input
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexture);

	// Progressively downsample through the mip chain
	for (int i = 0; i < (int)mipChain.size(); i++)
	{
		const BloomMip& mip = mipChain[i];
		glViewport(0, 0, mip.size.x, mip.size.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0);

		// Render screen-filled quad of resolution of current mip
		renderQuad();

		// Set current mip resolution as srcResolution for next iteration
		m_downSampleShader->SetVector2f("srcResolution", mip.size);
		// Set current mip as texture input for next iteration
		glBindTexture(GL_TEXTURE_2D, mip.texture);
		// Disable Karis average for consequent downsamples
		if (i == 0) { m_downSampleShader->SetInteger("mipLevel", 1); }
	}

	glUseProgram(0);
}

void BloomPass::renderUpSamples(float filterRadius)
{
	const std::vector<BloomMip>& mipChain = m_fbo.MipChain();

	m_upSampleShader->Use();
	m_upSampleShader->SetFloat("filterRadius", filterRadius);

	// Enable additive blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	for (int i = (int)mipChain.size() - 1; i > 0; i--)
	{
		const BloomMip& mip = mipChain[i];
		const BloomMip& nextMip = mipChain[i-1];

		// Bind viewport and texture from where to read
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mip.texture);

		// Set framebuffer render target (we write to this texture)
		glViewport(0, 0, nextMip.size.x, nextMip.size.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextMip.texture, 0);

		// Render screen-filled quad of resolution of current mip
		renderQuad();
	}

	// Disable additive blending
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

	glUseProgram(0);
}

void BloomPass::renderQuad()
{
    if (m_quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &m_quadVAO);
        glGenBuffers(1, &m_quadVBO);
        glBindVertexArray(m_quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}