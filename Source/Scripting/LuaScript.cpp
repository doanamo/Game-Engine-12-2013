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

LuaScript::LuaScript(std::string filename) :
    LuaScript()
{
    Load(filename);
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

std::string LuaScript::DetachStem(std::string& compoundVariable)
{
    // Find the separator.
    std::size_t separator = compoundVariable.find_first_of('.');

    // Copy the stem.
    std::string detached(compoundVariable, 0, separator);

    // Erase detached stem.
    if(separator != std::string::npos)
    {
        compoundVariable.erase(0, separator + 1);
    }
    else
    {
        compoundVariable.erase();
    }

    return detached;
}

bool LuaScript::PushVariable(std::string compoundVariable)
{
    assert(m_state);

    // Push compound on the stack.
    std::string element;

    while(true)
    {
        // Detach element name from variable compound.
        element = DetachStem(compoundVariable);

        if(element.empty())
            break;

        // Push variable on the stack.
        if(lua_gettop(m_state) == 0)
        {
            lua_getglobal(m_state, element.c_str());
        }
        else
        {
            lua_getfield(m_state, -1, element.c_str());
        }

        // Check if we pushed nil.
        if(lua_isnil(m_state, -1))
            return false;
    }

    return true;
}

void LuaScript::PopStack()
{
    assert(m_state);

    // Pop the entire stack.
    int size = lua_gettop(m_state);
    lua_pop(m_state, size);
}

std::string LuaScript::GetString(std::string compoundVariable, std::string defaultValue)
{
    std::string value = defaultValue;

    if(m_state)
    {
        if(PushVariable(compoundVariable))
        {
            if(lua_isstring(m_state, -1))
            {
                value = lua_tostring(m_state, -1);
            }
            
            PopStack();
        }
    }

    return value;
}

bool LuaScript::GetBool(std::string compoundVariable, bool defaultValue)
{
    bool value = defaultValue;

    if(m_state)
    {
        if(PushVariable(compoundVariable))
        {
            if(lua_isboolean(m_state, -1))
            {
                value = lua_toboolean(m_state, -1) != 0;
            }
            
            PopStack();
        }
    }

    return value;
}

int LuaScript::GetInteger(std::string compoundVariable, int defaultValue)
{
    int value = defaultValue;

    if(m_state)
    {
        if(PushVariable(compoundVariable))
        {
            if(lua_isnumber(m_state, -1))
            {
                value = (int)lua_tonumber(m_state, -1);
            }
            
            PopStack();
        }
    }

    return value;
}

float LuaScript::GetFloat(std::string compoundVariable, float defaultValue)
{
    float value = defaultValue;

    if(m_state)
    {
        if(PushVariable(compoundVariable))
        {
            if(lua_isnumber(m_state, -1))
            {
                value = (float)lua_tonumber(m_state, -1);
            }
            
            PopStack();
        }
    }

    return value;
}

bool LuaScript::IsValid() const
{
    return m_state != nullptr;
}
