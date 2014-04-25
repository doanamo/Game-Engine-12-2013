#include "Precompiled.hpp"
#include "GameStages.hpp"

#include "Game/GameContext.hpp"
#include "Game/GameFactory.hpp"
#include "Game/Spawn/SpawnSystem.hpp"
#include "Game/Progress/ProgressSystem.hpp"

//
// Enemy Stage
//

void EnemyStage::OnEnter()
{
    // Setup stage spawns.
    Game::SpawnSystem().AddSpawn(&SpawnFunction, 0.5f, 1.0f);

    // Setup next stage.
    Game::ProgressSystem().SetNextStage(std::make_shared<EnemyStage>());
}

void EnemyStage::OnExit()
{
    Game::SpawnSystem().ResetSpawns();
}

void EnemyStage::SpawnFunction(const glm::vec2& position)
{
    Game::CreateEnemy(position);
}
