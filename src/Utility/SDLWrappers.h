#pragma once
#include <memory>
#include <SDL.h>

struct SDLWindowDeleter
{
    void operator() (SDL_Window* window)
    {
        if (window != nullptr)
        {
            SDL_DestroyWindow(window);
        }
    }
};
using SDLWindowPtr = std::unique_ptr<SDL_Window, SDLWindowDeleter>;
inline std::shared_ptr<SDL_Window> MakeSharedSDLWindowPtr(SDL_Window* window)
{
    return std::shared_ptr<SDL_Window>(window, SDLWindowDeleter{});
}

struct SDLRendererDeleter
{
    void operator() (SDL_Renderer* renderer)
    {
        if (renderer != nullptr)
        {
            SDL_DestroyRenderer(renderer);
        }
    }
};
using SDLRendererPtr = std::unique_ptr<SDL_Renderer, SDLRendererDeleter>;

struct SDLTextureDeleter
{
    void operator() (SDL_Texture* texture)
    {
        if (texture != nullptr)
        {
            SDL_DestroyTexture(texture);
        }
    }
};
using SDLTexturePtr = std::unique_ptr<SDL_Texture, SDLTextureDeleter>;