#pragma once

class PostProcessPass
{
public:
    PostProcessPass();
    ~PostProcessPass();
    void Render(class Scene *scene, unsigned int pbrTexture, unsigned int bloomTexture);

private:
    unsigned int m_quadVAO, m_quadVBO;
    class Shader *m_shader;

    void renderQuad();
};