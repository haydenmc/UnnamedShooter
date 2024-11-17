#pragma once
#include "Configuration.h"
#include "Renderer.h"

struct Display
{
    Display(const VideoResolution& resolution);
    ~Display();
    std::shared_ptr<SDL_Window> GetWindow();
private:
    VideoResolution const m_resolution;
    std::shared_ptr<SDL_Window> const m_window;
};