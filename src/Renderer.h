#pragma once
#include "Configuration.h"
#include "Simulation.h"

struct Renderer
{
    Renderer(std::shared_ptr<SDL_Window> window, VideoConfiguration const& resolution);
    void Render(SimulationState const& simulationState);
private:
    std::shared_ptr<SDL_Window> const m_window;
    VideoConfiguration const m_resolution;
    SDLRendererPtr const m_renderer;
    std::vector<uint32_t> m_frameBuffer;
    SDLTexturePtr const m_frameBufferTexture;
    Matrix4x4 const m_projectionMatrix;

    void ClearBuffer();
    void ClearBuffer(uint32_t color);
    void DrawPixel(uint16_t x, uint16_t y, uint32_t color);
    void DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
    void DrawLine(Vector2 a, Vector2 b, uint32_t color);
    void DrawTriangle(Vector2 a, Vector2 b, Vector2 c, uint32_t color);
    void DrawScene(CameraEntity const* cameraEntity, Entity const* sceneEntity);
    void DrawEntityTreeMeshes(Matrix4x4 const& viewMatrix, Entity const* rootEntity);
    void DrawEntityMesh(Matrix4x4 const& viewMatrix, Entity const* entity, Mesh const* mesh);
};