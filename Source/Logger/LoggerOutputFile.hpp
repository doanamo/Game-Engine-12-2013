#pragma once

#include "Precompiled.hpp"
#include "LoggerOutput.hpp"

//
// Logger Output File
//

class LoggerOutputFile : public LoggerOutput
{
public:
    LoggerOutputFile();
    ~LoggerOutputFile();

    bool Open(std::string filename);
    void Cleanup();

    void Write(const LoggerMessage& message);

    bool IsOpen() const
    {
        return m_file.is_open();
    }

private:
    std::ofstream m_file;
};
