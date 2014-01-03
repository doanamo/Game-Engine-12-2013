#pragma once

#include "Precompiled.hpp"

//
// Frame Counter
//

class FrameCounter
{
public:
    FrameCounter();
    ~FrameCounter();

    bool Initialize(int historySize);
    void Cleanup();

    void Update(float dt);

    float GetFrameTime() const;
    float GetFrameRate() const;

private:
    float*  m_history;
    int     m_historySize;
    int     m_historyPosition;

    bool m_initialized;
};
