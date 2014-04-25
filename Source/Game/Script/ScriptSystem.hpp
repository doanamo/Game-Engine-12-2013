#pragma once

#include "Precompiled.hpp"

//
// Script System
//

class ScriptSystem
{
public:
    ScriptSystem();
    ~ScriptSystem();

    bool Initialize();
    void Cleanup();

    void Update(float timeDelta);

private:
    //
};
