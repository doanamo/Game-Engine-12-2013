#include "Precompiled.hpp"
#include "LuaLogger.hpp"
#include "LuaEngine.hpp"

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

bool BindLuaLogger(LuaEngine& state)
{
    if(!state.IsValid())
        return false;

    // Bind log function.
    Lua::getGlobalNamespace(state.GetState())
        .addFunction("Log", LuaLog);

    return true;
}
