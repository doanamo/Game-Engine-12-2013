#include "Precompiled.hpp"
#include "LuaEngine.hpp"

#include "MainGlobal.hpp"

namespace
{
    // Log error messages.
    #define LogInitializeError() "Failed to initialize Lua state! "
    #define LogLoadError(filename) "Failed to load \"" << filename << "\" file! "
}

LuaEngine::LuaEngine() :
    m_state(nullptr)
{
}

LuaEngine::~LuaEngine()
{
    Cleanup();
}

void LuaEngine::Cleanup()
{
    if(m_state)
    {
        lua_close(m_state);
        m_state = nullptr;
    }
}

bool LuaEngine::Initialize()
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

    // Load standard Lua libraries.
    luaL_openlibs(m_state);

    // Load other utility libraries.
    if(luaL_dostring(m_state, "bit = require(\"bit\")") != 0)
    {
        Log() << LogInitializeError() << "Coundn't load one of the libraries.";
        return false;
    }

    // Set random seed.
    if(luaL_dostring(m_state, "math.randomseed(os.time())") != 0)
    {
        Log() << LogInitializeError() << "Coundn't set random seed.";
        return false;
    }

    // Success!
    return initialized = true;
}

bool LuaEngine::Load(std::string filename)
{
    if(m_state == nullptr)
        return false;

    // Load Lua file.
    if(luaL_loadfile(m_state, (Main::GetWorkingDir() + filename).c_str()) != 0)
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

void LuaEngine::SetPackagePath(std::string path)
{
    if(!m_state)
        return;

    // Set path to package location (script working directiory).
    Lua::LuaRef package = GetVariable("package");
    package["path"] = path + "?.lua";
}

std::string LuaEngine::DetachStem(std::string& compoundVariable)
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

Lua::LuaRef LuaEngine::GetVariable(std::string compoundVariable)
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

std::string LuaEngine::GetString(std::string compoundVariable, std::string defaultValue)
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

bool LuaEngine::GetBool(std::string compoundVariable, bool defaultValue)
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

int LuaEngine::GetInteger(std::string compoundVariable, int defaultValue)
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

float LuaEngine::GetFloat(std::string compoundVariable, float defaultValue)
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

lua_State* LuaEngine::GetState()
{
    return m_state;
}

bool LuaEngine::IsValid() const
{
    return m_state != nullptr;
}
