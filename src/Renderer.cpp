#include <pch.h>
#include "Configuration.h"
#include "Renderer.h"

namespace
{
    constexpr uint32_t c_defaultBackgroundColor{ 0xFFFF0000 };

    SDLRendererPtr CreateRenderer(SDL_Window* window)
    {
        SPDLOG_INFO("Creating SDL Renderer");
        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
        CheckSdlPtr(renderer);
        return SDLRendererPtr{ renderer };
    }

    SDLTexturePtr CreateFrameBufferTexture(SDL_Renderer* renderer, VideoResolution resolution)
    {
        SPDLOG_INFO("Creating framebuffer texture");
        SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING, resolution.Width, resolution.Height);
        CheckSdlPtr(texture);
        return SDLTexturePtr{ texture };
    }
}

Renderer::Renderer(std::shared_ptr<SDL_Window> window, const VideoResolution& resolution) :
    m_window{ window }, m_resolution{ resolution }, m_renderer{ CreateRenderer(m_window.get()) },
    m_frameBuffer( static_cast<size_t>(m_resolution.Width * m_resolution.Height), c_defaultBackgroundColor ),
    m_frameBufferTexture{ CreateFrameBufferTexture(m_renderer.get(), m_resolution) }
{}

void Renderer::Render(SimulationState const& simulationState)
{
    ClearBuffer();
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
