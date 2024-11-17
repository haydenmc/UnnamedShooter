#pragma once
#include "Simulation.h"

using IntPoint2D = glm::vec<2, uint16_t>;
using FixedPoint2D = glm::vec<2, fpm::fixed_16_16>;

struct Renderer
{
    Renderer(std::shared_ptr<SDL_Window> window, const VideoConfiguration& resolution);
    void Render(SimulationState const& simulationState);
private:
    std::shared_ptr<SDL_Window> const m_window;
    VideoConfiguration const m_resolution;
    SDLRendererPtr const m_renderer;
    std::vector<uint32_t> m_frameBuffer;
    SDLTexturePtr const m_frameBufferTexture;

    void ClearBuffer();
    void ClearBuffer(uint32_t color);
    void DrawPixel(uint16_t x, uint16_t y, uint32_t color);
    void DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
    void DrawTriangle(IntPoint2D a, IntPoint2D b, IntPoint2D c, uint32_t color);
};