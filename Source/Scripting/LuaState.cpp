#include "Precompiled.hpp"
#include "LuaState.hpp"

#include "MainGlobal.hpp"

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize Lua state! "
    #define LogLoadError(filename) "Failed to load \"" << filename << "\" file! "
}

LuaState::LuaState() :
    m_state(nullptr)
{
}

LuaState::~LuaState()
{
    Cleanup();
}

bool LuaState::Initialize()
{
    Cleanup();

    // Initialization state.
    bool initialized = false;

    // Setup a scope guard.
    SCOPE_GUARD_IF(!initialized, Cleanup());

    // Create Lua state.
    m_state = luaL_newstate();

    if(m_state == nullptr)
    {
        Log() << LogInitializeError() << "Call returned a null.";
        return false;
    }

    // Success!
    return initialized = true;
}

bool LuaState::Load(std::string filename)
{
    if(m_state == nullptr)
        return false;

    // Load Lua file.
    if(luaL_loadfile(m_state, (Main::GetWorkingDir() + filename).c_str()) != 0)
    {
        Log() << LogLoadError(filename) << "Couldn't parse the file.";
        return false;
    }

    // Execute Lua state once.
    if(lua_pcall(m_state, 0, 0, 0) != 0)
    {
        std::string error = "Unknown error";

        if(lua_isstring(m_state, -1))
        {
            error = lua_tostring(m_state, -1);

            // Remove base path to working directory.
            std::size_t position = error.find(Main::GetWorkingDir());

            if(position != std::string::npos)
            {
                error.erase(position, Main::GetWorkingDir().size());
            }
        }

        Log() << "Lua error - " << error << ".";
        lua_pop(m_state, lua_gettop(m_state));
        return false;
    }

    return true;
}

void LuaState::Cleanup()
{
    if(m_state)
    {
        lua_close(m_state);
        m_state = nullptr;
    }
}

std::string LuaState::DetachStem(std::string& compoundVariable)
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

Lua::LuaRef LuaState::GetVariable(std::string compoundVariable)
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

std::string LuaState::GetString(std::string compoundVariable, std::string defaultValue)
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

bool LuaState::GetBool(std::string compoundVariable, bool defaultValue)
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

int LuaState::GetInteger(std::string compoundVariable, int defaultValue)
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

float LuaState::GetFloat(std::string compoundVariable, float defaultValue)
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

lua_State* LuaState::GetState()
{
    return m_state;
}

bool LuaState::IsValid() const
{
    return m_state != nullptr;
}
