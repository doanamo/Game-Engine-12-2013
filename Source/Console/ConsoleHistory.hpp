#pragma once

#include "Precompiled.hpp"

//
// Console History
//  Tracks history of recent console output and input.
//

class ConsoleHistory
{
public:
    ConsoleHistory();
    ~ConsoleHistory();

    bool Initialize(int outputCapacity, int inputCapacity);
    void Cleanup();

    // Console output.
    void WriteOutput(const char* text);
    void ClearOutput();

    const char* GetOutput(int index) const;

    // Console input.
    void WriteInput(const char* text);
    void ClearInput();

    const char* GetInput(int index) const;

    // Accessor methods.
    int GetOutputSize() const
    {
        return m_outputHistory.size();
    }

    int GetInputSize() const
    {
        return m_inputHistory.size();
    } 

    int GetOutputCapacity() const
    {
        return m_outputCapacity;
    }

    int GetInputCapacity() const
    {
        return m_inputCapacity;
    }

    bool IsOutputEmpty() const
    {
        return m_outputHistory.empty();
    }

    bool IsInputEmpty() const
    {
        return m_inputHistory.empty();
    }

    bool IsValid() const
    {
        return m_initialized;
    }

private:
    typedef std::deque<std::string> StringQueue;

    StringQueue m_outputHistory;
    int         m_outputCapacity;

    StringQueue m_inputHistory;
    int         m_inputCapacity;

    bool m_initialized;
};
