#pragma once

#include "Precompiled.hpp"

// Forward declarations.
class LuaState;
class Services;

//
// Lua Game
//

bool BindLuaGame(LuaState& state, const Services& services);
