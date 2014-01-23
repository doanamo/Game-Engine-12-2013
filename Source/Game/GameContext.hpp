#pragma once

#include "Precompiled.hpp"
#include "Common/StateMachine.hpp"
#include "ComponentContainer.hpp"

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

    ComponentContainer<Transform>& TransformComponents();
    ComponentContainer<Input>&     InputComponents();
    ComponentContainer<Script>&    ScriptComponents();
    ComponentContainer<Render>&    RenderComponents();

    StateMachine<BaseFrame>& StateMachine();
    MenuFrame&               MenuFrame();
    GameFrame&               GameFrame();
}
