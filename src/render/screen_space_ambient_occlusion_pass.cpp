#include "render/pass/screen_space_ambient_occlusion_pass.h"
#include "render/pass/geometry_pass.h"
#include "resource_manager.h"

#include <glad/glad.h>
#include <random>

SSAOPass::SSAOPass(unsigned int width, unsigned int height) 
: m_width(width), m_height(height),  m_ssaoFBO(0), m_ssaoTexture(0), m_ssaoBlurFBO(0), m_ssaoBlurTexture(0), m_noiseTexture(0), m_quadVAO(0), m_quadVBO(0)
{
    // load shader
    ResourceManager::LoadShader("shaders/sample_shader/9.ssao.vert", "shaders/sample_shader/9.ssao.frag", nullptr, nullptr, nullptr, "SSAO");
    ResourceManager::LoadShader("shaders/sample_shader/9.ssao.vert", "shaders/sample_shader/9.ssao_blur.frag", nullptr, nullptr, nullptr, "SSAOBlur");
    // configure uniforms
    // ssao shader
    ResourceManager::GetShader("SSAO").Use();
    ResourceManager::GetShader("SSAO").SetInteger("gDepth", 0);
    ResourceManager::GetShader("SSAO").SetInteger("gNormal", 1);
    ResourceManager::GetShader("SSAO").SetInteger("texNoise", 0);
    // kernal info
    ResourceManager::GetShader("SSAO").SetInteger("kernelSize", KERNAL_SIZE);
    ResourceManager::GetShader("SSAO").SetFloat("radius", RADIUS);
    ResourceManager::GetShader("SSAO").SetFloat("bias", BIAS);
    std::vector<glm::vec3> ssaoKernel = generateSampleKernal();
    for (unsigned int i = 0; i < KERNAL_SIZE; ++i) 
    {
        ResourceManager::GetShader("SSAO").SetVector3f(("samples[" + std::to_string(i) + "]").c_str(), ssaoKernel[i]);
    }
    // blur shader
    ResourceManager::GetShader("SSAOBlur").Use();
    ResourceManager::GetShader("SSAO").SetInteger("ssaoInput", 0);
    // SSAO color buffer
    glGenFramebuffers(1, &m_ssaoFBO); 
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO);
    glGenTextures(1, &m_ssaoTexture);
    glBindTexture(GL_TEXTURE_2D, m_ssaoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssaoTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // Blur color buffer
    glGenFramebuffers(1, &m_ssaoBlurFBO);
    glGenTextures(1, &m_ssaoBlurTexture);
    glBindTexture(GL_TEXTURE_2D, m_ssaoBlurTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);  
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ssaoBlurTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // generate noise
    std::vector<glm::vec3> noise = generateNoise();
    // noise texture
    glGenTextures(1, &m_noiseTexture);
    glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &noise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set external access data
    m_totalData.id = m_ssaoBlurTexture;
}

SSAOPass::~SSAOPass()
{
    // framebuffers
    if(!m_ssaoFBO) glDeleteFramebuffers(1, &m_ssaoFBO);
    if(!m_ssaoBlurFBO) glDeleteFramebuffers(1, &m_ssaoBlurFBO);
    m_ssaoFBO = m_ssaoBlurFBO = 0;
    // textures
    if(!m_ssaoTexture) glDeleteTextures(1, &m_ssaoTexture);
    if(!m_ssaoBlurTexture) glDeleteTextures(1, &m_ssaoBlurTexture);
    if(!m_noiseTexture) glDeleteTextures(1, &m_noiseTexture);
    m_ssaoTexture = m_ssaoBlurTexture = m_noiseTexture = 0;
    // delete buffers
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteBuffers(1, &m_quadVBO);
}

void SSAOPass::Render(GBufferTextures& gBufferTextures)
{
    // generate ssao texture
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    ResourceManager::GetShader("SSAO").Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBufferTextures.depth);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBufferTextures.normal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
    renderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // blur ssao texture
    glBindFramebuffer(GL_FRAMEBUFFER, m_ssaoBlurFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    ResourceManager::GetShader("SSAOBlur").Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ssaoTexture);
    renderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAOPass::renderQuad()
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

#pragma region utility functions
// Generate a small random float
float SSAOPass::randomFloat()
{
    static std::default_random_engine generator;
    static std::uniform_real_distribution<float> distrib(0.0f, 1.0f);
    return distrib(generator);
}

float SSAOPass::ourLerp(float a, float b, float f)
{
    return a + f * (b - a);
}

std::vector<glm::vec3> SSAOPass::generateSampleKernal()
{
    std::vector<glm::vec3> kernel;
    // generate sample kernal
    for (unsigned int i = 0; i < KERNAL_SIZE; ++i)
    {
        glm::vec3 sample(randomFloat() * 2.0 - 1.0, randomFloat() * 2.0 - 1.0, randomFloat());
        sample = glm::normalize(sample);
        sample *= randomFloat();
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = ourLerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        kernel.push_back(sample);
    }
    return kernel;
}

std::vector<glm::vec3> SSAOPass::generateNoise()
{
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloat() * 2.0 - 1.0, randomFloat()  * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    return ssaoNoise;
}

#pragma endregion