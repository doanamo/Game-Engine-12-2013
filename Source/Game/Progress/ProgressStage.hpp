#pragma once

#include "Precompiled.hpp"

#include "Common/StateMachine.hpp"

//
// Progress Stage
//

class ProgressStage : public State<>
{
public:
    friend class ProgressSystem;

public:
    virtual float GetStageLength() const = 0;

private:
};
