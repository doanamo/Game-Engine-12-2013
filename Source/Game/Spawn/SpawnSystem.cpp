#include "Precompiled.hpp"
#include "SpawnSystem.hpp"

#include "MainGlobal.hpp"
#include "Game/GameFactory.hpp"

namespace
{
    std::random_device randomDevice;
    std::mt19937 spawnRandom(randomDevice());
    std::mt19937 coordRandom(randomDevice());
}

SpawnSystem::SpawnSystem()
{
}

SpawnSystem::~SpawnSystem()
{
    Cleanup();
}

bool SpawnSystem::Initialize()
{
    Cleanup();

    return true;
}

void SpawnSystem::Cleanup()
{
    m_spawnList.clear();
}

void SpawnSystem::Update(float timeDelta)
{
    // Traverse the spawn definition list.
    for(auto& definition : m_spawnList)
    {
        // Update the current delay timer.
        definition.currentDelay = std::max(0.0f, definition.currentDelay - timeDelta);

        // Use definition to spawn a new entity.
        if(definition.currentDelay == 0)
        {
            // Calculate spawn area size.
            glm::vec2 size;
            size.x = definition.area.z - definition.area.x;
            size.y = definition.area.w - definition.area.y;

            assert(size.x >= 0 && size.y >= 0);

            // Pick a random point inside the spawn area.
            glm::vec2 position;
            position.x = definition.area.x + std::uniform_real<float>(0.0f, size.x)(coordRandom);
            position.y = definition.area.y + std::uniform_real<float>(0.0f, size.y)(coordRandom);

            // Call the spawn function.
            try
            {
                definition.function(position);
            }
            catch(Lua::LuaException& exception)
            {
                // Get the exception error text.
                std::string error = exception.what();

                // Remove base path to working directory.
                std::size_t position = error.find(Main::GetWorkingDir());

                if(position != std::string::npos)
                {
                    error.erase(position, Main::GetWorkingDir().size());
                }

                // Print the error.
                Log() << "Lua error - " << error << ".";
            }

            // Set a new spawn delay.
            definition.currentDelay = std::uniform_real<float>(definition.minDelay, definition.maxDelay)(spawnRandom);
        }
    }
}

void SpawnSystem::AddSpawn(Lua::LuaRef function, const glm::vec4& area, float minDelay, float maxDelay)
{
    // Validate function.
    if(!function.isFunction())
        return;

    // Create a spawn definition.
    SpawnDefinition definition(function, area, minDelay, maxDelay);

    // Roll the initial delay.
    definition.currentDelay = std::uniform_real<float>(minDelay, maxDelay)(spawnRandom);

    // Add a definition to the list.
    m_spawnList.push_back(definition);
}

void SpawnSystem::RemoveAllSpawns()
{
    m_spawnList.clear();
}
