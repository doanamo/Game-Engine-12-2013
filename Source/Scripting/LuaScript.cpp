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

Lua::LuaRef LuaScript::GetVariable(std::string compoundVariable)
{
    assert(m_state);

    // Get the lua variable.
    Lua::LuaRef variable(m_state);

    while(true)
    {
        // Detach variable name from variable compound.
        std::string name = DetachStem(compoundVariable);

        if(name.empty())
            break;

        // Get variable reference.
        if(variable.isNil())
        {
            variable = Lua::getGlobal(m_state, name.c_str());
        }
        else
        {
            variable = Lua::LuaRef(variable[name.c_str()]);
        }

        // Check if we got nil.
        if(variable.isNil())
            break;
    }

    return variable;
}

std::string LuaScript::GetString(std::string compoundVariable, std::string defaultValue)
{
    std::string value = defaultValue;

    if(m_state)
    {
        Lua::LuaRef variable = GetVariable(compoundVariable);

        if(variable.isString())
        {
            value = variable.cast<std::string>();
        }
    }

    return value;
}

bool LuaScript::GetBool(std::string compoundVariable, bool defaultValue)
{
    bool value = defaultValue;

    if(m_state)
    {
        Lua::LuaRef variable = GetVariable(compoundVariable);

        if(variable.isBoolean())
        {
            value = variable.cast<bool>();
        }
    }

    return value;
}

int LuaScript::GetInteger(std::string compoundVariable, int defaultValue)
{
    int value = defaultValue;

    if(m_state)
    {
        Lua::LuaRef variable = GetVariable(compoundVariable);

        if(variable.isNumber())
        {
            value = variable.cast<int>();
        }
    }

    return value;
}

float LuaScript::GetFloat(std::string compoundVariable, float defaultValue)
{
    float value = defaultValue;

    if(m_state)
    {
        Lua::LuaRef variable = GetVariable(compoundVariable);

        if(variable.isNumber())
        {
            value = variable.cast<float>();
        }
    }

    return value;
}

bool LuaScript::IsValid() const
{
    return m_state != nullptr;
}
