#include "Precompiled.hpp"
#include "FrameCounter.hpp"

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize a frame counter! "
}

FrameCounter::FrameCounter() :
    m_history(nullptr),
    m_historySize(0),
    m_historyPosition(0),
    m_initialized(false)
{
}

FrameCounter::~FrameCounter()
{
    Cleanup();
}

bool FrameCounter::Initialize(int historySize)
{
    Cleanup();

    // Validate arguments.
    if(historySize <= 0)
    {
        Log() << LogInitializeError() << "Invalid arguments - \"historySize\" is invalid.";
        Cleanup();
        return false;
    }

    m_historySize = historySize;

    // Allocate history buffer.
    m_history = new float[m_historySize];

    if(m_history == nullptr)
    {
        Log() << LogInitializeError() << "Couldn't allocate a history buffer.";
        Cleanup();
        return false;
    }

    // Clear the buffer.
    memset(&m_history[0], 0, sizeof(float) * m_historySize);

    m_initialized = true;

    return true;
}

void FrameCounter::Cleanup()
{
    delete[] m_history;
    m_history = nullptr;

    m_historySize = 0;
    m_historyPosition = 0;

    m_initialized = false;
}

void FrameCounter::Update(float dt)
{
    if(!m_initialized)
        return;

    // Set current frame time.
    m_history[m_historyPosition] = dt;

    // Advance next history position write.
    ++m_historyPosition;

    if(m_historyPosition >= m_historySize)
    {
        m_historyPosition = 0;
    }
}

float FrameCounter::GetFrameTime() const
{
    if(!m_initialized)
        return 0.0f;

    float accumulator = 0.0f;

    for(int i = 0; i < m_historySize; ++i)
    {
        accumulator += m_history[i];
    }

    return accumulator / (float)m_historySize;
}

float FrameCounter::GetFrameRate() const
{
    if(!m_initialized)
        return 0.0f;

    return 1.0f / GetFrameTime();
}
