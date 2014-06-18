#include "Precompiled.hpp"
#include "ConsoleHistory.hpp"

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize a console history! "
}

ConsoleHistory::ConsoleHistory() :
    m_outputCapacity(0),
    m_inputCapacity(0),
    m_initialized(false)
{
}

ConsoleHistory::~ConsoleHistory()
{
    Cleanup();
}

bool ConsoleHistory::Initialize(int outputCapacity, int inputCapacity)
{
    Cleanup();

    // Validate arguments.
    if(outputCapacity <= 0)
    {
        Log() << LogInitializeError() << "Invalid argument - \"outputCapacity\" is invalid.";
        return false;
    }

    if(inputCapacity <= 0)
    {
        Log() << LogInitializeError() << "Invalid argument - \"inputCapacity\" is invalid.";
        return false;
    }

    m_outputCapacity = outputCapacity;
    m_inputCapacity = inputCapacity;

    // Success!
    m_initialized = true;

    return true;
}

void ConsoleHistory::Cleanup()
{
    ClearContainer(m_outputHistory);
    ClearContainer(m_inputHistory);

    m_outputCapacity = 0;
    m_inputCapacity = 0;

    m_initialized = false;
}

void ConsoleHistory::WriteOutput(const char* text)
{
    if(!m_initialized)
        return;

    if(text == nullptr)
        return;

    // Check if there is space for another string.
    assert((int)m_outputHistory.size() <= m_outputCapacity);

    if((int)m_outputHistory.size() == m_outputCapacity)
    {
        // Remove oldest string.
        m_outputHistory.pop_back();
    }

    // Add string to the history.
    m_outputHistory.emplace_front(text);
}

void ConsoleHistory::ClearOutput()
{
    if(!m_initialized)
        return;

    // Clear output strings.
    ClearContainer(m_outputHistory);
}

const char* ConsoleHistory::GetOutput(int index) const
{
    if(IsOutputEmpty())
        return nullptr;

    if(index < 0)
        return nullptr;

    if(index >= (int)m_outputHistory.size())
        return nullptr;

    // Return string at index.
    return m_outputHistory[index].c_str();
}

void ConsoleHistory::WriteInput(const char* text)
{
    if(!m_initialized)
        return;

    if(text == nullptr)
        return;

    // Check if there is space for another string.
    assert((int)m_inputHistory.size() <= m_inputCapacity);

    if((int)m_inputHistory.size() == m_inputCapacity)
    {
        // Remove oldest string.
        m_inputHistory.pop_back();
    }

    // Add string to the history.
    m_inputHistory.emplace_front(text);
}

void ConsoleHistory::ClearInput()
{
    if(!m_initialized)
        return;

    // Clear output strings.
    ClearContainer(m_inputHistory);
}

const char* ConsoleHistory::GetInput(int index) const
{
    if(IsInputEmpty())
        return nullptr;

    if(index < 0)
        return nullptr;

    if(index >= (int)m_inputHistory.size())
        return nullptr;

    // Return string at index.
    return m_inputHistory[index].c_str();
}
