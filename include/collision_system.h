#pragma once

#include <vector>
#include <unordered_set>

class Collider;
class World;

struct ColliderPair
{
    Collider* a;
    Collider* b;

    ColliderPair(Collider* a_, Collider* b_)
    {
        if(a_ < b_) { a = a_; b = b_; }
        else { a = b_; b = a_; }
    }

    bool operator==(const ColliderPair& other) const
    {
        return a == other.a && b == other.b;
    }
};

struct ColliderPairHash
{
    size_t operator()(const ColliderPair& p) const
    {
        size_t h1 = std::hash<Collider*>()(p.a);
        size_t h2 = std::hash<Collider*>()(p.b);

        return h1 ^ (h2 << 1);
    }
};

class CollisionSystem
{
public:
    void Step(const World& world);
private:
    void broadPhase(const World& world);
    void narrowPhase();
    void dispatchCollisionEvents();
    std::unordered_set<ColliderPair, ColliderPairHash> m_pairs;
    std::unordered_set<Collider*> m_allColliders;
};