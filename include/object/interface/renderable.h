#pragma once

class Renderable
{
public:
    virtual ~Renderable() = default;
    virtual void Render() = 0;
    virtual void RenderShadow() = 0;
};