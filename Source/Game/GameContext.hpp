#pragma once

#include "Precompiled.hpp"
#include "Common/StateMachine.hpp"
#include "ComponentPool.hpp"

// Forward declarations.
class InputState;

class EntitySystem;
class ScriptSystem;
class RenderSystem;

class Transform;
class Input;
class Script;
class Render;

class BaseFrame;
class MenuFrame;
class GameFrame;

//
// Game Context
//

namespace Game
{
    // Context methods.
    bool Initialize();
    void Cleanup();

    // Context accessors.
    bool IsInitialized();

    InputState& InputState();

    EntitySystem& EntitySystem();
    ScriptSystem& ScriptSystem();
    RenderSystem& RenderSystem();

    ComponentPool<Transform>& TransformComponents();
    ComponentPool<Input>&     InputComponents();
    ComponentPool<Script>&    ScriptComponents();
    ComponentPool<Render>&    RenderComponents();

    StateMachine<BaseFrame>& StateMachine();
    MenuFrame&               MenuFrame();
    GameFrame&               GameFrame();
}
