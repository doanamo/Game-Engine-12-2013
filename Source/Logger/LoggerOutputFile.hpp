#pragma once

#include "Precompiled.hpp"
#include "LoggerOutput.hpp"

//
// Logger Output File
//

class LoggerOutputFile : public LoggerOutput
{
public:
	LoggerOutputFile(std::string filename);
	~LoggerOutputFile();

	bool Open(std::string filename);
	void Close();

	void Write(const LoggerMessage& message);

private:
	std::ofstream m_file;
};
