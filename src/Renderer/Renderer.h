#pragma once
#include "../Configuration.h"
#include "../Overlay/Overlay.h"
#include "RenderTarget.h"
#include "../Simulation.h"

enum class FrustumPlaneKind
{
    LeftFrustumPlane,
    RightFrustumPlane,
    TopFrustumPlane,
    BottomFrustumPlane,
    NearFrustumPlane,
    FarFrustumPlane,
    MAX
};

namespace game
{
struct Renderer
{
    Renderer(std::shared_ptr<SDL_Window> window, VideoConfiguration const& resolution);
    void Render(SimulationState const& simulationState);
    void AddOverlay(std::shared_ptr<Overlay> overlay);

private:
    std::shared_ptr<SDL_Window> const m_window;
    VideoConfiguration const m_resolution;
    SDLRendererPtr const m_renderer;
    RenderTarget m_frameBuffer;
    SDLTexturePtr const m_frameBufferTexture;
    Eigen::Matrix4f const m_projectionMatrix;
    std::unordered_map<FrustumPlaneKind, Plane> const m_frustumPlanes; // TODO: Again, should be generated by/from the Camera entity.
    std::vector<std::shared_ptr<Overlay>> m_overlays;

    void DrawScene(CameraEntity const* cameraEntity, Entity const* sceneEntity);
    void DrawEntityTreeMeshes(Eigen::Matrix4f const& viewMatrix, Entity const* rootEntity);
    void DrawEntityMesh(Eigen::Matrix4f const& viewMatrix, Entity const* entity, Mesh const* mesh);
};
}