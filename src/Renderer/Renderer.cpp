#include <pch.h>
#include "Configuration.h"
#include "../Mesh/Mesh.h"
#include "Renderer.h"

namespace
{
    constexpr float c_defaultFovRads{ 3.14159265f / 2.0f };
    constexpr float c_nearPlane{ 0.1f };
    constexpr float c_farPlane{ 100.0f };

    SDLRendererPtr CreateRenderer(SDL_Window* window)
    {
        SPDLOG_INFO("Creating SDL Renderer");
        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        CheckSdlPtr(renderer);
        CheckSdlReturn(SDL_RenderSetIntegerScale(renderer, SDL_TRUE));
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

    Matrix4x4 CreatePerspectiveMatrix(float fovYRads, float width, float height,
        float nearPlane, float farPlane)
    {
        return Matrix4x4::PerspectiveProjection(FixedUnit{ fovYRads }, FixedUnit{ height / width },
            FixedUnit{ nearPlane }, FixedUnit{ farPlane });
    }
}

Renderer::Renderer(std::shared_ptr<SDL_Window> window, const VideoConfiguration& resolution) :
    m_window{ window }, m_resolution{ resolution }, m_renderer{ CreateRenderer(m_window.get()) },
    m_frameBuffer{ m_resolution.Width, m_resolution.Height },
    m_frameBufferTexture{ CreateFrameBufferTexture(m_renderer.get(), m_resolution) },
    m_projectionMatrix{ CreatePerspectiveMatrix(c_defaultFovRads, m_resolution.Width,
        m_resolution.Height, c_nearPlane, c_farPlane) }
{}

void Renderer::Render(SimulationState const& simulationState)
{
    m_frameBuffer.ClearBuffer();
    // Draw
    // DrawTriangle({ 64, 32 }, { 32, 64 }, { 96, 64 }, 0xFF0000FF);
    // DrawTriangle({ 128, 32 }, { 64, 32 }, { 96, 64 }, 0xFF00FFFF);
    // m_frameBuffer.DrawTriangle(
    //     Vector2{ FixedUnit{ 4 }, FixedUnit { 1 } }, //A
    //     Vector2{ FixedUnit{ 1 }, FixedUnit { 1 } }, //B
    //     Vector2{ FixedUnit{ 1 }, FixedUnit { 4 } }, //C
    //     0xFFFF0000);

    // m_frameBuffer.DrawTriangle(
    //     Vector2{ FixedUnit{ 5 }, FixedUnit { 5 } }, //D
    //     Vector2{ FixedUnit{ 4 }, FixedUnit { 1 } }, //A
    //     Vector2{ FixedUnit{ 1 }, FixedUnit { 4 } }, //C
    //     0xFF00FF00);


    // m_frameBuffer.DrawTriangle(
    //     Vector2{ FixedUnit{ 140 }, FixedUnit { 100 } }, //A
    //     Vector2{ FixedUnit{ 140 }, FixedUnit { 40 } }, //B
    //     Vector2{ FixedUnit{ 80 }, FixedUnit { 40 } }, //C
    //     0xFF00FF00);

    // // m_frameBuffer.DrawTriangle(
    // //     Vector2{ FixedUnit{ 140 }, FixedUnit { 100 } }, //A
    // //     Vector2{ FixedUnit{ 80 }, FixedUnit { 40 } }, //C
    // //     Vector2{ FixedUnit{ 50 }, FixedUnit { 90 } }, //C
    // //     0xFF0000FF);

    DrawScene(&simulationState.Camera, &simulationState.RootWorldEntity);
    
    CheckSdlReturn(SDL_UpdateTexture(m_frameBufferTexture.get(), nullptr,
        m_frameBuffer.Buffer.data(), (m_resolution.Width * sizeof(uint32_t))));
    CheckSdlReturn(SDL_RenderCopy(m_renderer.get(), m_frameBufferTexture.get(), nullptr, nullptr));
    SDL_RenderPresent(m_renderer.get());
}

void Renderer::DrawScene(CameraEntity const *cameraEntity, Entity const *sceneEntity)
{
    // Calculate view/camera matrix
    // TODO: Respect camera rotation
    auto viewMatrix{ Matrix4x4::LookAt(cameraEntity->GetPosition(),
        Vector3{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 1 } },
        Vector3{ FixedUnit{ 0 }, FixedUnit{ 1 }, FixedUnit{ 0 } }) };
    DrawEntityTreeMeshes(viewMatrix, sceneEntity);
}

void Renderer::DrawEntityTreeMeshes(Matrix4x4 const& viewMatrix, Entity const* rootEntity)
{
    for (const auto& child : rootEntity->GetChildren())
    {
        DrawEntityTreeMeshes(viewMatrix, child.get());
    }
    for (const auto& mesh : rootEntity->GetMeshes())
    {
        DrawEntityMesh(viewMatrix, rootEntity, mesh.get());
    }
}

void Renderer::DrawEntityMesh(Matrix4x4 const& viewMatrix, Entity const* entity, Mesh const *mesh)
{
    // First apply entity transformations
    auto entityTranslate{ Matrix4x4::Translation(entity->GetPosition()) };
    auto entityRotate { Matrix4x4::EulerRotation(entity->GetRotation()) };

    // Vertex points
#if FALSE
    for (const auto& vertex : mesh->Vertices)
    {
        Vector4 v{ vertex.x, vertex.y, vertex.z, FixedUnit{ 1 } };
        auto transformedVertex{ m_projectionMatrix * viewMatrix * entityTranslate * entityRotate * v };
        auto screenX{ (transformedVertex.x / transformedVertex.w) * FixedUnit{ m_resolution.Width / 2.0f } +
            FixedUnit{ m_resolution.Width / 2.0f } + FixedUnit{ 0.5f } };
        auto screenY{ (transformedVertex.y / transformedVertex.w) * FixedUnit{ m_resolution.Height / 2.0f } +
            FixedUnit{ m_resolution.Height / 2.0f } + FixedUnit{ 0.5f } };
        DrawPixel(static_cast<uint16_t>(screenX), static_cast<uint16_t>(screenY), 0xFFFFFFFF);
    }
#endif

    // Wireframe
#if FALSE
    for (const auto& face : mesh->Faces)
    {
        std::array<glm::vec3, 3> faceVertices{
            mesh->Vertices.at(face.MeshVertexIndices.at(0)),
            mesh->Vertices.at(face.MeshVertexIndices.at(1)),
            mesh->Vertices.at(face.MeshVertexIndices.at(2)),
        };

        std::array<glm::vec4, 3> transformedVertices{
            m_projectionMatrix * viewMatrix * entityTransform * glm::vec4{ faceVertices.at(0).x, faceVertices.at(0).y, faceVertices.at(0).z, 1.0f },
            m_projectionMatrix * viewMatrix * entityTransform * glm::vec4{ faceVertices.at(1).x, faceVertices.at(1).y, faceVertices.at(1).z, 1.0f },
            m_projectionMatrix * viewMatrix * entityTransform * glm::vec4{ faceVertices.at(2).x, faceVertices.at(2).y, faceVertices.at(2).z, 1.0f },
        };

        std::array<Vector2, 3> screenSpaceCoordinates{
            Vector2{ (transformedVertices.at(0).x / transformedVertices.at(0).w) * (m_resolution.Width / 2.0f) + (m_resolution.Width / 2.0f),
                (transformedVertices.at(0).y / transformedVertices.at(0).w) * (m_resolution.Height / 2.0f) + (m_resolution.Height / 2.0f) },
            Vector2{ (transformedVertices.at(1).x / transformedVertices.at(1).w) * (m_resolution.Width / 2.0f) + (m_resolution.Width / 2.0f),
                (transformedVertices.at(1).y / transformedVertices.at(1).w) * (m_resolution.Height / 2.0f) + (m_resolution.Height / 2.0f) },
            Vector2{ (transformedVertices.at(2).x / transformedVertices.at(2).w) * (m_resolution.Width / 2.0f) + (m_resolution.Width / 2.0f),
                (transformedVertices.at(2).y / transformedVertices.at(2).w) * (m_resolution.Height / 2.0f) + (m_resolution.Height / 2.0f) },
        };

        DrawLine(screenSpaceCoordinates.at(0), screenSpaceCoordinates.at(1), 0xFF00FF00);
        DrawLine(screenSpaceCoordinates.at(1), screenSpaceCoordinates.at(2), 0xFF00FF00);
        DrawLine(screenSpaceCoordinates.at(2), screenSpaceCoordinates.at(0), 0xFF00FF00);
    }
#endif

    // Shaded
#if TRUE
    std::array<uint32_t, 6> faceColors{
        0xFFFF0000,
        0xFF00FF00,
        0xFF0000FF,
        0xFFFFFF00,
        0xFFFF00FF,
        0xFF00FFFF,
    };
    size_t faceNum = 0;
    for (const auto& face : mesh->Faces)
    {
        const auto& pointA{ mesh->Vertices.at(face.MeshVertexIndices.at(0)) };
        const auto& pointB{ mesh->Vertices.at(face.MeshVertexIndices.at(1)) };
        const auto& pointC{ mesh->Vertices.at(face.MeshVertexIndices.at(2)) };

        std::array<Vector4, 3> transformedVertices{
            m_projectionMatrix * viewMatrix * entityTranslate * entityRotate * Vector4{ pointA.x, pointA.y, pointA.z, FixedUnit{ 1 } },
            m_projectionMatrix * viewMatrix * entityTranslate * entityRotate * Vector4{ pointB.x, pointB.y, pointB.z, FixedUnit{ 1 } },
            m_projectionMatrix * viewMatrix * entityTranslate * entityRotate * Vector4{ pointC.x, pointC.y, pointC.z, FixedUnit{ 1 } },
        };
        FixedUnit halfWidth{ m_resolution.Width / 2.0f };
        FixedUnit halfHeight{ m_resolution.Height / 2.0f };
        std::array<Vector2, 3> screenSpaceCoordinates{
            Vector2{ (transformedVertices.at(0).x / transformedVertices.at(0).w) * halfWidth + halfWidth,
                (transformedVertices.at(0).y / transformedVertices.at(0).w) * halfHeight + halfHeight },
            Vector2{ (transformedVertices.at(1).x / transformedVertices.at(1).w) * halfWidth + halfWidth,
                (transformedVertices.at(1).y / transformedVertices.at(1).w) * halfHeight + halfHeight },
            Vector2{ (transformedVertices.at(2).x / transformedVertices.at(2).w) * halfWidth + halfWidth,
                (transformedVertices.at(2).y / transformedVertices.at(2).w) * halfHeight + halfHeight },
        };
        // SPDLOG_DEBUG("Triangle @ ({},{}), ({},{}), ({},{})",
        //     static_cast<float>(screenSpaceCoordinates.at(0).x),static_cast<float>(screenSpaceCoordinates.at(0).y),
        //     static_cast<float>(screenSpaceCoordinates.at(1).x),static_cast<float>(screenSpaceCoordinates.at(1).y),
        //     static_cast<float>(screenSpaceCoordinates.at(2).x),static_cast<float>(screenSpaceCoordinates.at(2).y));
        const uint32_t& color{ faceColors.at(faceNum % faceColors.size()) };
        m_frameBuffer.DrawTriangle(screenSpaceCoordinates.at(0), screenSpaceCoordinates.at(1), screenSpaceCoordinates.at(2), color);
        ++faceNum;
    }
#endif
}


