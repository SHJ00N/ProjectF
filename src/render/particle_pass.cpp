#include "render/pass/particle_pass.h"
#include "scene/scene.h"
#include "particle/particle_manager.h"

void ParticlePass::Render(Scene *scene, unsigned int depthTexture, int width, int height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if(scene->GetParticleManager()) scene->GetParticleManager()->Render(depthTexture, width, height);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}