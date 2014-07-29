#pragma once

#include "Precompiled.hpp"

//
// Lua Script
//

class LuaScript
{
public:
    LuaScript();
    ~LuaScript();

    bool Load(std::string filename);
    void Cleanup();

    bool IsValid() const;

private:
    lua_State* m_state;
};
