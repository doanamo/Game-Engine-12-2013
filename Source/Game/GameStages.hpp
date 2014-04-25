#pragma once

#include "Precompiled.hpp"

#include "Game/Progress/ProgressStage.hpp"

//
// Enemy Stage
//

class EnemyStage : public ProgressStage
{
public:
    void OnEnter();
    void OnExit();

    float GetStageLength() const
    {
        return 30.0f;
    }

private:
    static void SpawnFunction(const glm::vec2& position);
};
