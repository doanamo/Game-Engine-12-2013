#pragma once

#include "Precompiled.hpp"

//
// Lua Engine
//

class LuaEngine
{
public:
    LuaEngine();
    ~LuaEngine();

    bool Initialize();
    bool Load(std::string filename);
    void Cleanup();

    template<typename... Arguments>
    Lua::LuaRef Call(std::string compoundVariable, Arguments... arguments);

    void SetPackagePath(std::string path);

    Lua::LuaRef GetReference(std::string compoundVariable);
    std::string GetString(std::string compoundVariable, std::string defaultValue = "nil");
    bool GetBool(std::string compoundVariable, bool defaultValue = false);
    int GetInteger(std::string compoundVariable, int defaultValue = 0);
    float GetFloat(std::string compoundVariable, float defaultValue = 0.0f);

    lua_State* GetState();

    bool IsValid() const;

private:
    std::string DetachStem(std::string& compoundVariable);

private:
    lua_State* m_state;
};

//
// Template Implementation
//

template<typename... Arguments>
Lua::LuaRef LuaEngine::Call(std::string compoundVariable, Arguments... arguments)
{
    // Create nil reference for results.
    Lua::LuaRef results(m_state);

    // Get the function variable.
    Lua::LuaRef function = GetReference(compoundVariable);

    // Call the function.
    try
    {
        results = function(arguments...);
    }
    catch(Lua::LuaException& exception)
    {
        // Get the exception error text.
        std::string error = exception.what();

        // Remove base path to working directory.
        std::size_t position = error.find(Main::GetWorkingDir());

        if(position != std::string::npos)
        {
            error.erase(position, Main::GetWorkingDir().size());
        }

        // Print the error.
        Log() << "Lua error - " << error << ".";
    }

    // Return results.
    return results;
}
