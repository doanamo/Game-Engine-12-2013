#pragma once

#include "Precompiled.hpp"

// Forward declarations.
class LuaEngine;
class Services;

//
// Lua Game
//

bool BindLuaGame(LuaEngine& lua, const Services& services);
