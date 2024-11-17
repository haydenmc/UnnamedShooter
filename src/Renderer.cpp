#include <pch.h>
#include "Configuration.h"
#include "Renderer.h"

namespace
{
    SDLRendererPtr CreateRenderer(SDL_Window* window)
    {
        SPDLOG_INFO("Creating SDL Renderer");
        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
        if (renderer == nullptr)
        {
            throw new std::runtime_error(fmt::format("Failed to create SDL Renderer: {}",
                SDL_GetError()));
        }
        return SDLRendererPtr{ renderer };
    }

    SDLTexturePtr CreateFrameBufferTexture(SDL_Renderer* renderer, VideoResolution resolution)
    {
        SPDLOG_INFO("Creating framebuffer texture");
        SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING, resolution.Width, resolution.Height);
        if (texture == nullptr)
        {
            throw new std::runtime_error(fmt::format("Failed to create SDL Texture: {}",
                SDL_GetError()));
        }
        return SDLTexturePtr{ texture };
    }
}

Renderer::Renderer(std::shared_ptr<SDL_Window> window, const VideoResolution& resolution) :
    m_window{ window }, m_resolution{ resolution }, m_renderer{ CreateRenderer(m_window.get()) },
    m_frameBufferTexture{ CreateFrameBufferTexture(m_renderer.get(), m_resolution) }
{ }