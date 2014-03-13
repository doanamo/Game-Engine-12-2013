#pragma once

//
// Standard C++
//

#include <cassert>
#include <memory>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <queue>

//
// Dependencies
//

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// SDL
#include <SDL.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H

// UTF8
#include <utf8.h>

//
// Project
//

#include "Common/NonCopyable.hpp"
#include "Common/ScopeGuard.hpp"
#include "Common/Utility.hpp"

#include "Logger/Logger.hpp"

#include "Console/ConsoleCommand.hpp"
#include "Console/ConsoleVariable.hpp"
