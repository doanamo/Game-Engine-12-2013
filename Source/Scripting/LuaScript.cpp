#include "Precompiled.hpp"
#include "LuaScript.hpp"

namespace
{
    // Log error messages.
    #define LogLoadError(filename) "Failed to load \"" << filename << "\" file! "
}

LuaScript::LuaScript() :
    m_state(nullptr)
{
}

LuaScript::~LuaScript()
{
    Cleanup();
}

bool LuaScript::Load(std::string filename)
{
    Cleanup();

    // Initialization state.
    bool initialized = false;

    // Create new Lua state.
    m_state = luaL_newstate();
    SCOPE_GUARD_IF(!initialized, Cleanup());

    // Load Lua file.
    if(luaL_loadfile(m_state, filename.c_str()) != 0)
    {
        Log() << LogLoadError(filename) << "Couldn't parse the file.";
        return false;
    }

    // Execute Lua state once.
    if(lua_pcall(m_state, 0, 0, 0) != 0)
    {
        Log() << LogLoadError(filename) << "Couldn't execute the state.";
        return false;
    }

    // Success!
    return initialized = true;
}

void LuaScript::Cleanup()
{
    if(m_state)
    {
        lua_close(m_state);
        m_state = nullptr;
    }
}

bool LuaScript::IsValid() const
{
    return m_state != nullptr;
}
