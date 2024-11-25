#pragma once
#include "Configuration.h"
#include "Renderer/Renderer.h"

struct Display
{
    Display(const VideoConfiguration& configuration);
    ~Display();
    std::shared_ptr<SDL_Window> GetWindow();
private:
    VideoConfiguration const m_configuration;
    std::shared_ptr<SDL_Window> const m_window;
};