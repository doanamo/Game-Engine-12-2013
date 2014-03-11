#include "Precompiled.hpp"
#include "GameStages.hpp"

#include "GameContext.hpp"
#include "GameFactory.hpp"

#include "SpawnSystem.hpp"
#include "ProgressSystem.hpp"

//
// Enemy Stage
//

void EnemyStage::OnEnter()
{
    // Setup stage spawns.
    Game::SpawnSystem().AddSpawn(&SpawnFunction, 0.5f, 1.0f);

    // Setup next stage.
    Game::ProgressSystem().SetNextStage(std::make_shared<AsteroidStage>());
}

void EnemyStage::OnExit()
{
    Game::SpawnSystem().ResetSpawns();
}

void EnemyStage::SpawnFunction(const glm::vec2& position)
{
    Game::CreateEnemy(position);
}

//
// Asteroid Stage
//

void AsteroidStage::OnEnter()
{
    // Setup stage spawns.
    Game::SpawnSystem().AddSpawn(&SpawnFunction, 0.5f, 1.0f);

    // Setup next stage.
    Game::ProgressSystem().SetNextStage(std::make_shared<EnemyStage>());
}

void AsteroidStage::OnExit()
{
    Game::SpawnSystem().ResetSpawns();
}

void AsteroidStage::SpawnFunction(const glm::vec2& position)
{
    Game::CreateAsteroid(position);
}
