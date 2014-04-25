#include "Precompiled.hpp"
#include "ProgressSystem.hpp"

ProgressSystem::ProgressSystem() :
    m_stageTime(0.0f)
{
}

ProgressSystem::~ProgressSystem()
{
}

bool ProgressSystem::Initialize()
{
    Cleanup();

    return true;
}

void ProgressSystem::Cleanup()
{
    m_stageCurrent.Cleanup();

    m_stageNext = nullptr;
    m_stageTime = 0.0f;
}

void ProgressSystem::Update(float timeDelta)
{
    auto ChangeStage = [&]()
    {
        // Extract the next stage.
        StagePtr nextStage = m_stageNext;
        m_stageNext = nullptr;

        // Change current stage.
        m_stageCurrent.ChangeState(nextStage);

        // Reset stage time.
        m_stageTime = 0.0f;
    };

    if(m_stageCurrent.IsValid())
    {
        if(m_stageTime < m_stageCurrent.GetCurrentState()->GetStageLength())
        {
            // Increment current stage time.
            m_stageTime += timeDelta;
        }
        else
        {
            ChangeStage();
        }
    }
    else
    {
        if(m_stageNext != nullptr)
        {
            ChangeStage();
        }
    } 
}

void ProgressSystem::SetNextStage(StagePtr stage)
{
    m_stageNext = stage;
}
