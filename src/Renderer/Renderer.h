#pragma once
#include "Configuration.h"
#include "RenderTarget.h"
#include "Simulation.h"

struct Renderer
{
    Renderer(std::shared_ptr<SDL_Window> window, VideoConfiguration const& resolution);
    void Render(SimulationState const& simulationState);

private:
    std::shared_ptr<SDL_Window> const m_window;
    VideoConfiguration const m_resolution;
    SDLRendererPtr const m_renderer;
    RenderTarget m_frameBuffer;
    SDLTexturePtr const m_frameBufferTexture;
    Matrix4x4 const m_projectionMatrix;

    void DrawScene(CameraEntity const* cameraEntity, Entity const* sceneEntity);
    void DrawEntityTreeMeshes(Matrix4x4 const& viewMatrix, Entity const* rootEntity);
    void DrawEntityMesh(Matrix4x4 const& viewMatrix, Entity const* entity, Mesh const* mesh);
};