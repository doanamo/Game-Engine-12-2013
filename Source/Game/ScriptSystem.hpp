#pragma once

#include "Precompiled.hpp"
#include "EntitySubsystem.hpp"

//
// Script System
//

class ScriptSystem : public EntitySubsystem
{
public:
    ScriptSystem();
    ~ScriptSystem();

    bool Initialize();
    void Cleanup();

private:
    // Processes an entity.
    void Process(Entity* entity);

private:
    //
};
