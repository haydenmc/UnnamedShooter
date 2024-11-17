#include <pch.h>
#include "Configuration.h"
#include "Renderer.h"

namespace
{
    constexpr fpm::fixed_16_16 c_zero { 0 };
    constexpr uint32_t c_defaultBackgroundColor{ 0xFFFF0000 };

    SDLRendererPtr CreateRenderer(SDL_Window* window)
    {
        SPDLOG_INFO("Creating SDL Renderer");
        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
        CheckSdlPtr(renderer);
        return SDLRendererPtr{ renderer };
    }

    SDLTexturePtr CreateFrameBufferTexture(SDL_Renderer* renderer, VideoConfiguration resolution)
    {
        SPDLOG_INFO("Creating framebuffer texture");
        SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING, resolution.Width, resolution.Height);
        CheckSdlPtr(texture);
        return SDLTexturePtr{ texture };
    }

    fpm::fixed_16_16 TriangleDeterminant(FixedPoint2D pointA, FixedPoint2D pointB,
        FixedPoint2D pointC)
    {
        const auto ab{ pointB - pointA };
        const auto ac{ pointC - pointA };

        return ((ab.y * ac.x) - (ab.x * ac.y));
    }

    bool IsTriangleEdgeLeftOrTop(FixedPoint2D pointA, FixedPoint2D pointB)
    {
        const auto edge{ pointB - pointA };
        const bool isLeftEdge{ edge.y > fpm::fixed_16_16{ 0 } };
        const bool isTopEdge{ (edge.y == fpm::fixed_16_16{ 0 }) &&
            (edge.x < fpm::fixed_16_16{ 0 }) };
        return (isLeftEdge || isTopEdge);
    }
}

Renderer::Renderer(std::shared_ptr<SDL_Window> window, const VideoConfiguration& resolution) :
    m_window{ window }, m_resolution{ resolution }, m_renderer{ CreateRenderer(m_window.get()) },
    m_frameBuffer( static_cast<size_t>(m_resolution.Width * m_resolution.Height), c_defaultBackgroundColor ),
    m_frameBufferTexture{ CreateFrameBufferTexture(m_renderer.get(), m_resolution) }
{}

void Renderer::Render(SimulationState const& simulationState)
{
    ClearBuffer();
    // Draw
    DrawTriangle({ 64, 32 }, { 32, 64 }, { 96, 64 }, 0xFF0000FF);
    DrawTriangle({ 128, 32 }, { 64, 32 }, { 96, 64 }, 0xFF00FFFF);
    CheckSdlReturn(SDL_UpdateTexture(m_frameBufferTexture.get(), nullptr, m_frameBuffer.data(),
        (m_resolution.Width * sizeof(uint32_t))));
    CheckSdlReturn(SDL_RenderCopy(m_renderer.get(), m_frameBufferTexture.get(), nullptr, nullptr));
    SDL_RenderPresent(m_renderer.get());
}

void Renderer::ClearBuffer()
{
    ClearBuffer(c_defaultBackgroundColor);
}

void Renderer::ClearBuffer(uint32_t color)
{
    std::fill(m_frameBuffer.begin(), m_frameBuffer.end(), color);
}

void Renderer::DrawPixel(uint16_t x, uint16_t y, uint32_t color)
{
    m_frameBuffer[(m_resolution.Width * y) + x] = color;
}

void Renderer::DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
{
    uint16_t minX{ std::min(x1, x2) };
    uint16_t maxX{ std::max(x1, x2) };
    uint16_t minY{ std::min(y1, y2) };
    uint16_t maxY{ std::max(y1, y2) };
    for (uint16_t y{ minY }; y <= maxY; ++y)
    {
        for (uint16_t x{ minX }; x <= maxX; ++x)
        {
            DrawPixel(x, y, color);
        }
    }
}

void Renderer::DrawTriangle(IntPoint2D a, IntPoint2D b, IntPoint2D c, uint32_t color)
{
    // Confirm vertices are provided in counter-clockwise order
    if (TriangleDeterminant(a, b, c) <= c_zero)
    {
        SPDLOG_WARN("Invalid winding order of triangle vertices ({},{}), ({},{}), ({},{})",
            a.x, a.y, b.x, b.y, c.x, c.y);
        return;
    }

    // Simple rasterization - test every pixel of the rectangular boundary
    // surrounding the triangle and fill the points "inside" the triangle edges
    const auto xMin = std::min(a.x, std::min(b.x, c.x));
    const auto yMin = std::min(a.y, std::min(b.y, c.y));
    const auto xMax = std::max(a.x, std::max(b.x, c.x));
    const auto yMax = std::max(a.y, std::max(b.y, c.y));
    for (auto y{ yMin }; y <= yMax; ++y)
    {
        for (auto x{ xMin }; x <= xMax; ++x)
        {
            IntPoint2D point{ x, y };

            glm::vec<3, fpm::fixed_16_16> edges{
                TriangleDeterminant(b, c, point),
                TriangleDeterminant(c, a, point),
                TriangleDeterminant(a, b, point)
            };

            // Apply top-left edge rule
            if (IsTriangleEdgeLeftOrTop(b, c))
            {
                edges.x -= 1;
            }
            if (IsTriangleEdgeLeftOrTop(c, a))
            {
                edges.y -= 1;
            }
            if (IsTriangleEdgeLeftOrTop(a, b))
            {
                edges.z -= 1;
            }
            
            if ((edges.x >= c_zero) && (edges.y >= c_zero) && (edges.z >= c_zero))
            {
                DrawPixel(point.x, point.y, color);
            }
        }
    }
}
