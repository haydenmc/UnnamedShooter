#include <pch.h>
#include "Display.h"

namespace
{
    std::shared_ptr<SDL_Window> CreateSDLWindow(VideoResolution resolution)
    {
        SPDLOG_INFO("Initializing SDL");
        CheckSdlReturn(SDL_InitSubSystem(SDL_INIT_VIDEO));

        SPDLOG_INFO("Creating {}x{} window", resolution.Width, resolution.Height);
        SDL_Window* window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, resolution.Width, resolution.Height, 0);
        CheckSdlPtr(window);
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
