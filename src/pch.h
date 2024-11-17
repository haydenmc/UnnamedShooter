// C++ standard library
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>

// Windows
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// Spdlog
#include <spdlog/spdlog.h>

// SDL
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "Utility/SDLWrappers.h"

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>