#pragma once

class Renderable
{
public:
    virtual ~Renderable() = default;
    virtual void Render(const struct Frustum& frustum) = 0;
    virtual void RenderShadow() = 0;
};