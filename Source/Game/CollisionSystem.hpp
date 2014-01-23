#pragma once

#include "Precompiled.hpp"

//
// Collision System
//

class CollisionSystem
{
public:
    CollisionSystem();
    ~CollisionSystem();

    bool Initialize();
    void Cleanup();

    void Update();

private:
    //
};
