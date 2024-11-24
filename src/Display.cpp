#include <pch.h>
#include "Display.h"

namespace
{
    std::shared_ptr<SDL_Window> CreateSDLWindow(VideoConfiguration configuration)
    {
        SPDLOG_INFO("Initializing SDL video subsystem");
        CheckSdlReturn(SDL_InitSubSystem(SDL_INIT_VIDEO));

        SPDLOG_INFO("Creating {}x{} window", configuration.Width, configuration.Height);
        SDL_Window* window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, configuration.Width, configuration.Height, 0);
        CheckSdlPtr(window);

        if (configuration.IsFullscreen)
        {
            CheckSdlReturn(SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP));
        }

        return MakeSharedSDLWindowPtr(window);
    }
}

Display::Display(const VideoConfiguration& configuration) : m_configuration{ configuration },
    m_window{ CreateSDLWindow(m_configuration) }
{ }

Display::~Display()
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

std::shared_ptr<SDL_Window> Display::GetWindow()
{
    return m_window;
}
