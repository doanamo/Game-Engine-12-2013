#pragma once

//
// Collision Types
//

struct CollisionTypes
{
    enum Type
    {
        None        = 0,
        Player      = 1 << 0,
        Enemy       = 1 << 1,
        Projectile  = 1 << 2,
        Pickup      = 1 << 3,
        Environment = 1 << 4,
    };
};

//
// Collision Masks
//

struct CollisionMasks
{
    static const uint32_t All = ~0;
};