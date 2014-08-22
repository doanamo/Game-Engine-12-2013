#include "Precompiled.hpp"
#include "LuaBindings.hpp"
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

bool BindLuaLogger(LuaEngine& lua)
{
    if(!lua.IsValid())
        return false;

    // Bind log function.
    Lua::getGlobalNamespace(lua.GetState())
        .addFunction("Log", LuaLog);

    return true;
}
