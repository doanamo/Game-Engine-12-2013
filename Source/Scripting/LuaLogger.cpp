#include "Precompiled.hpp"
#include "LuaLogger.hpp"

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

void BindLuaLogger(lua_State* state)
{
    assert(state != nullptr);

    // Bind log function.
    Lua::getGlobalNamespace(state)
        .addFunction("Log", LuaLog);
}
