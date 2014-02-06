#pragma once

#include "Precompiled.hpp"
#include "Common/StateMachine.hpp"
#include "ComponentPool.hpp"

// Forward declarations.
class InputState;

class EntitySystem;
class CollisionSystem;
class ScriptSystem;
class RenderSystem;

class TransformComponent;
class CollisionComponent;
class InputComponent;
class ScriptComponent;
class RenderComponent;

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

    EntitySystem&    EntitySystem();
    CollisionSystem& CollisionSystem();
    ScriptSystem&    ScriptSystem();
    RenderSystem&    RenderSystem();

    ComponentPool<TransformComponent>& TransformComponents();
    ComponentPool<CollisionComponent>& CollisionComponents();
    ComponentPool<InputComponent>&     InputComponents();
    ComponentPool<ScriptComponent>&    ScriptComponents();
    ComponentPool<RenderComponent>&    RenderComponents();

    StateMachine<BaseFrame>& StateMachine();
    MenuFrame&               MenuFrame();
    GameFrame&               GameFrame();
}
