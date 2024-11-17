#include <pch.h>
#include "Display.h"

namespace
{
    std::shared_ptr<SDL_Window> CreateSDLWindow(VideoResolution resolution)
    {
        SPDLOG_INFO("Initializing SDL");
        if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
        {
            throw new std::runtime_error("Could not initialize SDL video subsystem");
        }

        SPDLOG_INFO("Creating {}x{} window", resolution.Width, resolution.Height);
        SDL_Window* window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, resolution.Width, resolution.Height, 0);
        if (window == nullptr)
        {
            throw new std::runtime_error(fmt::format("Failed to create SDL Window: {}",
                SDL_GetError()));
        }
        return MakeSharedSDLWindowPtr(window);
    }
}

Display::Display(const VideoResolution& resolution) : m_resolution{ resolution },
    m_window{ CreateSDLWindow(m_resolution) }
{ }

Display::~Display()
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

std::shared_ptr<SDL_Window> Display::GetWindow()
{
    return m_window;
}
