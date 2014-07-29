#pragma once

#include "Precompiled.hpp"

//
// Lua Script
//

class LuaScript
{
public:
    LuaScript();
    LuaScript(std::string filename);
    ~LuaScript();

    bool Load(std::string filename);
    void Cleanup();

    std::string GetString(std::string compound, std::string unavailable = "nil");
    bool GetBool(std::string compound, bool unavailable = false);
    int GetInteger(std::string compound, int unavailable = 0);
    float GetFloat(std::string compound, float unavailable = 0.0f);

    bool IsValid() const;

private:
    std::string DetachStem(std::string& compound);
    bool PushVariable(std::string compound);
    void PopStack();

private:
    lua_State* m_state;
    int m_level;
};
