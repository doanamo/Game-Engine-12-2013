#pragma once

// Standard C++
#include <cassert>
#include <iostream>
#include <string>
#include <map>

// GLM
#include <glm/glm.hpp>

// SDL2
#include <SDL.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// Project
#include "Common/NonCopyable.hpp"
#include "Common/ScopeGuard.hpp"
#include "Common/Utility.hpp"

#include "Logger.hpp"

#include "Context.hpp"
