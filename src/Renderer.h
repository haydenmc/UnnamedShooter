#pragma once
#include "Simulation.h"

struct Renderer
{
    Renderer(std::shared_ptr<SDL_Window> window, const VideoResolution& resolution);
    void Render(SimulationState const& simulationState);
private:
    std::shared_ptr<SDL_Window> const m_window;
    VideoResolution const m_resolution;
    SDLRendererPtr const m_renderer;
    std::vector<uint32_t> m_frameBuffer;
    SDLTexturePtr const m_frameBufferTexture;

    void ClearBuffer();
    void ClearBuffer(uint32_t color);
};