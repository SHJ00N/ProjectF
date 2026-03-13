#pragma once

#include "object/collider/aabb.h"
#include "world/chunk.h"

struct BoxCollider
{
    AABB worldAABB;
    class Chunk *currentChunk = nullptr;

    ~BoxCollider()
    {
        if(currentChunk) currentChunk->RemoveCollider(&worldAABB);
    }
};