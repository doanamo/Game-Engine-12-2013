#pragma once

//
// Standard C++
//

#include <cassert>
#include <iostream>
#include <string>
#include <map>

//
// Dependencies
//

// GLM
#include <glm/glm.hpp>

// SDL
#include <SDL.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

//
// Project
//

#include "Common/NonCopyable.hpp"
#include "Common/ScopeGuard.hpp"
#include "Common/Utility.hpp"

#include "Logger.hpp"

#include "Context.hpp"
