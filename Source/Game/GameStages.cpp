#include "Precompiled.hpp"
#include "GameStages.hpp"

#include "Game/GameContext.hpp"
#include "Game/GameState.hpp"
#include "Game/GameFactory.hpp"
#include "Game/Spawn/SpawnSystem.hpp"
#include "Game/Progress/ProgressSystem.hpp"

//
// Enemy Stage
//

void EnemyStage::OnEnter()
{
    // Setup stage spawns.
    GameState::SpawnSystem().AddSpawn(&SpawnFunction, 0.5f, 1.0f);

    // Setup next stage.
    GameState::ProgressSystem().SetNextStage(std::make_shared<EnemyStage>());
}

void EnemyStage::OnExit()
{
    GameState::SpawnSystem().ResetSpawns();
}

void EnemyStage::SpawnFunction(const glm::vec2& position)
{
    GameFactory::CreateEnemy(position);
}
