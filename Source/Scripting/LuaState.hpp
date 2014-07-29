#pragma once

#include "Precompiled.hpp"

//
// Lua State
//

class LuaState
{
public:
    LuaState();
    ~LuaState();

    bool Initialize();
    bool Load(std::string filename);
    void Cleanup();

    Lua::LuaRef GetVariable(std::string compoundVariable);
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
