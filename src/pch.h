// C++ standard library
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_map>
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
#ifdef DEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

// SDL
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "Utility/SDLWrappers.h"

// Eigen
#pragma warning(push, 0) // Disable all warnings
#include <Eigen/Core>
#include <Eigen/Geometry>
#pragma warning(pop)     // Restore the previous warning state

// FPM
#include <fpm/fixed.hpp>
#include <fpm/math.hpp>

// STB
#include <stb_image.h>

// Custom headers
#include "MathHelpers.h"
#include "Utility/LoggingHelpers.h"