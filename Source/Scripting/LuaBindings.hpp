#pragma once

#include "Precompiled.hpp"

// Forward declarations.
class LuaEngine;

//
// Lua Bindings
//

bool BindLuaMath(LuaEngine& lua);
bool BindLuaLogger(LuaEngine& lua);
bool BindLuaConsole(LuaEngine& lua);
bool BindLuaSystem(LuaEngine& lua);
bool BindLuaGraphics(LuaEngine& lua);
bool BindLuaInterface(LuaEngine& lua);
bool BindLuaGame(LuaEngine& lua);
