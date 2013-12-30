#pragma once

//
// Standard C++
//

#include <cassert>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>

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

// UTF8
#include <utf8.h>

//
// Project
//

#include "Common/NonCopyable.hpp"
#include "Common/ScopeGuard.hpp"
#include "Common/Utility.hpp"

#include "Logger/Logger.hpp"

#include "Context.hpp"
