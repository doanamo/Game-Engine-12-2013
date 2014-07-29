#include "Precompiled.hpp"
#include "LuaLogger.hpp"

#include "LuaState.hpp"

namespace
{
    // Proxy log function.
    void LuaLog(const char* text)
    {
        LoggerMessage message;
        message.SetText(text);
        message.SetSource("Lua");

        Logger::GetGlobal()->Write(message);
    }
}

void BindLuaLogger(LuaState& state)
{
    assert(state.IsValid());

    // Bind log function.
    Lua::getGlobalNamespace(state.GetState())
        .addFunction("Log", LuaLog);
}
