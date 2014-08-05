#include "Precompiled.hpp"
#include "GameFrame.hpp"

#include "MainGlobal.hpp"
#include "Game/GameGlobal.hpp"
#include "Game/GameState.hpp"
#include "Game/GameFactory.hpp"
#include "Game/MenuFrame.hpp"
#include "Game/LoseFrame.hpp"
#include "Game/Entity/EntitySystem.hpp"
#include "Game/Identity/IdentitySystem.hpp"
#include "Game/Input/InputSystem.hpp"
#include "Game/Collision/CollisionSystem.hpp"
#include "Game/Script/ScriptSystem.hpp"
#include "Game/Render/RenderSystem.hpp"
#include "Game/Interface/InterfaceSystem.hpp"
#include "Game/Spawn/SpawnSystem.hpp"

// Temp!
#include "Scripting/LuaState.hpp"

namespace
{
    void SpawnFunction(const glm::vec2& position)
    {
        GameFactory::CreateEnemy(position);
    }
}

GameFrame::GameFrame() :
    m_initialized(false),
    m_playerDead(false),
    m_deadTimer(0.0f)
{
}

GameFrame::~GameFrame()
{
    Cleanup();
}

bool GameFrame::Initialize()
{
    Cleanup();

    // Setup emergency cleanup.
    auto EmergenyCleanup = MakeScopeGuard([&]()
    {
        // Cleanup if initialization failed.
        if(!m_initialized)
        {
            Cleanup();
        }
    });

    // Initialize the game state.
    if(!GameState::Initialize())
        return false;

    // Temp!
    Lua::getGlobalNamespace(Main::GetLuaState().GetState())
        .beginClass<EntityHandle>("EntityHandle")
            .addConstructor<void(*)(void)>()
            .addData("identifier", &EntityHandle::identifier, false)
            .addData("version", &EntityHandle::version, false)
        .endClass();

    Lua::getGlobalNamespace(Main::GetLuaState().GetState())
        .beginClass<EntitySystem>("EntitySystem")
            .addFunction("CreateEntity", &EntitySystem::CreateEntity)
            .addFunction("DestroyEntity", &EntitySystem::DestroyEntity)
        .endClass();

    Lua::getGlobalNamespace(Main::GetLuaState().GetState())
        .beginClass<IdentitySystem>("IdentitySystem")
            .addFunction("SetEntityName", &IdentitySystem::SetEntityName)
        .endClass();

    if(!Main::GetLuaState().Load("Data/Scripts/Main.lua"))
        return false;

    Lua::push(Main::GetLuaState().GetState(), &GameState::GetEntitySystem());
    lua_setglobal(Main::GetLuaState().GetState(), "EntitySystem");

    Lua::push(Main::GetLuaState().GetState(), &GameState::GetIdentitySystem());
    lua_setglobal(Main::GetLuaState().GetState(), "IdentitySystem");

    Main::GetLuaState().Call("GameState.Initialize");

    // Create bounds.
    GameFactory::CreateBounds();

    // Create the player.
    GameFactory::CreatePlayer();

    // Setup the spawn system.
    GameState::GetSpawnSystem().SetSpawnArea(glm::vec4(1024.0f + 100.0f, 50.0f, 1024.0f + 100.0f, 526.0f));
    GameState::GetSpawnSystem().AddSpawn(&SpawnFunction, 0.5f, 1.0f);

    // Success!
    m_initialized = true;

    return true;
}

void GameFrame::Cleanup()
{
    GameState::Cleanup();

    m_initialized = false;

    m_playerDead = false;
    m_deadTimer = 0.0f;
}

bool GameFrame::Process(const SDL_Event& event)
{
    switch(event.type)
    {
    case SDL_KEYDOWN:
        if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
            // Prevent from quitting to the main menu if player is dead.
            if(m_playerDead)
                break;

            // Change to main menu state.
            GameContext::GetFrameState().ChangeState(&GameContext::GetMenuFrame());
        }
        break;
    }

    // Process input events.
    GameState::GetInputSystem().Process(event);

    return false;
}

void GameFrame::Update(float timeDelta)
{
    // Update the spawn system.
    GameState::GetSpawnSystem().Update(timeDelta);

    // Process entity commands.
    GameState::GetEntitySystem().ProcessCommands();

    // Update the collision system.
    GameState::GetCollisionSystem().Update(timeDelta);

    // Update the script system.
    GameState::GetScriptSystem().Update(timeDelta);

    // Update the render system.
    GameState::GetRenderSystem().Update();

    // Update the interface system.
    GameState::GetInterfaceSystem().Update(timeDelta);

    // Check if player is dead.
    if(!m_playerDead)
    {
        EntityHandle player = GameState::GetIdentitySystem().GetEntityByName("Player");

        if(!GameState::GetEntitySystem().IsHandleValid(player))
        {
            // Set the dead state.
            m_playerDead = true;
        }
    }
    else
    {
        // Increment the dead timer.
        m_deadTimer += timeDelta;

        // Change to lose frame (game over screen).
        if(m_deadTimer > 4.0f)
        {
            GameContext::GetGameFrame().Cleanup();
            GameContext::GetFrameState().ChangeState(&GameContext::GetLoseFrame());
        }
    }
}

void GameFrame::Draw()
{
    // Draw the world.
    GameState::GetRenderSystem().Draw();

    // Draw the interface.
    GameState::GetInterfaceSystem().Draw();
}
