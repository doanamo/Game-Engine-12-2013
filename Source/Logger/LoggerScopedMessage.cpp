#include "Precompiled.hpp"
#include "LoggerScopedMessage.hpp"
#include "Logger.hpp"

LoggerScopedMessage::LoggerScopedMessage(Logger* logger) :
    m_logger(logger)
{
}

LoggerScopedMessage::LoggerScopedMessage(LoggerScopedMessage&& other) :
    LoggerMessage(std::forward<LoggerScopedMessage>(other))
{
    m_logger = other.m_logger;
    other.m_logger = nullptr;
}

LoggerScopedMessage::~LoggerScopedMessage()
{
    if(m_logger != nullptr)
    {
        m_logger->Write(*this);
    }
}
