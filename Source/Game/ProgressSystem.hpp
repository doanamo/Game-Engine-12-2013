#pragma once

#include "Precompiled.hpp"
#include "ProgressStage.hpp"
#include "Common/StateMachine.hpp"

//
// Progress System
//

class ProgressSystem
{
public:
    typedef std::shared_ptr<ProgressStage> StagePtr;

public:
    ProgressSystem();
    ~ProgressSystem();

    bool Initialize();
    void Cleanup();

    void Update(float timeDelta);

    void SetNextStage(StagePtr stage);

private:
    StateMachine<StagePtr> m_stageCurrent;
    StagePtr               m_stageNext;
    float                  m_stageTime;
};
