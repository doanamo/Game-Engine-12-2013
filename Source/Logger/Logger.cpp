#include "Precompiled.hpp"
#include "Logger.hpp"

#include <ctime>
#include <iomanip>

Logger::Logger()
{
}

Logger::~Logger()
{
    Cleanup();
}

void Logger::Cleanup()
{
    ClearContainer(m_outputs);
}

void Logger::AddOutput(LoggerOutput* output)
{
    if(output == nullptr)
        return;

    // Add logger output.
    m_outputs.push_back(output);
}

void Logger::RemoveOutput(LoggerOutput* output)
{
    if(output == nullptr)
        return;

    // Find and remove logger output.
    auto it = std::find(m_outputs.begin(), m_outputs.end(), output);

    if(it != m_outputs.end())
    {
        m_outputs.erase(it);
    }
}

void Logger::Write(const LoggerMessage& message)
{
    // Write to every logger output.
    auto it = m_outputs.begin();

    while(it != m_outputs.end())
    {
        (*it++)->Write(message);
    }
}
