#pragma once

struct Renderer
{
    Renderer(std::shared_ptr<SDL_Window> window, const VideoResolution& resolution);
private:
    std::shared_ptr<SDL_Window> const m_window;
    VideoResolution const m_resolution;
    SDLRendererPtr const m_renderer;
    SDLTexturePtr const m_frameBufferTexture;
};