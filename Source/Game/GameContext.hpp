#pragma once

#include "Precompiled.hpp"

#include "Common/StateMachine.hpp"
#include "Component/ComponentPool.hpp"

// Forward declarations.
class InputState;

class EntitySystem;
class IdentitySystem;
class HealthSystem;
class CollisionSystem;
class ScriptSystem;
class RenderSystem;
class InterfaceSystem;

class TransformComponent;
class InputComponent;
class HealthComponent;
class CollisionComponent;
class ScriptComponent;
class RenderComponent;

class SpawnSystem;
class ProgressSystem;

class BaseFrame;
class MenuFrame;
class GameFrame;
class LoseFrame;

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
    IdentitySystem&  IdentitySystem();
    HealthSystem&    HealthSystem();
    CollisionSystem& CollisionSystem();
    ScriptSystem&    ScriptSystem();
    RenderSystem&    RenderSystem();
    InterfaceSystem& InterfaceSystem();

    ComponentPool<TransformComponent>& TransformComponents();
    ComponentPool<InputComponent>&     InputComponents();
    ComponentPool<HealthComponent>&    HealthComponents();
    ComponentPool<CollisionComponent>& CollisionComponents();
    ComponentPool<ScriptComponent>&    ScriptComponents();
    ComponentPool<RenderComponent>&    RenderComponents();

    SpawnSystem&    SpawnSystem();
    ProgressSystem& ProgressSystem();

    StateMachine<BaseFrame*>& FrameState();
    MenuFrame&                MenuFrame();
    GameFrame&                GameFrame();
    LoseFrame&                LoseFrame();
}
