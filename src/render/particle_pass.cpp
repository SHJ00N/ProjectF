#include "render/pass/particle_pass.h"
#include "scene/scene.h"
#include "particle/particle_manager.h"

void ParticlePass::Render(Scene *scene, unsigned int framebuffer, int width, int height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if(scene->GetParticleManager()) scene->GetParticleManager()->Render(width, height);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}