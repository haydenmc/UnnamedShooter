// C++ standard library
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <initializer_list>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>

// Windows
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

// Spdlog
#include <spdlog/spdlog.h>

// SDL
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "Utility/SDLWrappers.h"

// FPM
#include <fpm/fixed.hpp>
#include <fpm/math.hpp>

// Custom headers
#include "Types.h"
#include "MathTypes.h"