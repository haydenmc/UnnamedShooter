#include <pch.h>
#include "Configuration.h"
#include "Mesh.h"
#include "Renderer.h"

namespace
{
    constexpr fpm::fixed_16_16 c_zero { 0 };
    constexpr uint32_t c_defaultBackgroundColor{ 0xFF000000 };
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

    FixedUnit TriangleDeterminant(Vector2 pointA,
        Vector2 pointB, Vector2 pointC)
    {
        const auto ab{ pointB - pointA };
        const auto ac{ pointC - pointA };

        return ((ab.y * ac.x) - (ab.x * ac.y));
    }

    bool IsTriangleEdgeLeftOrTop(Vector2 pointA, Vector2 pointB)
    {
        const auto edge{ pointB - pointA };
        const bool isLeftEdge{ edge.y > FixedUnit{ 0 } };
        const bool isTopEdge{ (edge.y == FixedUnit{ 0 }) && (edge.x < FixedUnit{ 0 }) };
        return (isLeftEdge || isTopEdge);
    }
}

Renderer::Renderer(std::shared_ptr<SDL_Window> window, const VideoConfiguration& resolution) :
    m_window{ window }, m_resolution{ resolution }, m_renderer{ CreateRenderer(m_window.get()) },
    m_frameBuffer( static_cast<size_t>(m_resolution.Width * m_resolution.Height),
        c_defaultBackgroundColor ),
    m_frameBufferTexture{ CreateFrameBufferTexture(m_renderer.get(), m_resolution) },
    m_projectionMatrix{ CreatePerspectiveMatrix(c_defaultFovRads, m_resolution.Width,
        m_resolution.Height, c_nearPlane, c_farPlane) }
{}

void Renderer::Render(SimulationState const& simulationState)
{
    ClearBuffer();
    // Draw
    // DrawTriangle({ 64, 32 }, { 32, 64 }, { 96, 64 }, 0xFF0000FF);
    // DrawTriangle({ 128, 32 }, { 64, 32 }, { 96, 64 }, 0xFF00FFFF);
    DrawScene(&simulationState.Camera, &simulationState.RootWorldEntity);
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

void Renderer::DrawLine(Vector2 a, Vector2 b, uint32_t color)
{
    std::array<FixedUnit, 2> fixedA{ FixedUnit{ a.x } + FixedUnit{ 0.5f },
        FixedUnit{ a.y } + FixedUnit{ 0.5f } };
    std::array<FixedUnit, 2> fixedB{ FixedUnit{ b.x } + FixedUnit{ 0.5f },
        FixedUnit{ b.y } + FixedUnit{ 0.5f } };

    FixedUnit deltaX{ fixedB.at(0) - fixedA.at(0) };
    FixedUnit deltaY{ fixedB.at(1) - fixedA.at(1) };
    FixedUnit sideLength{ fpm::abs(deltaX) >= fpm::abs(deltaY) ?
        fpm::abs(deltaX) : fpm::abs(deltaY) };

    FixedUnit xIncrement{ deltaX / sideLength };
    FixedUnit yIncrement{ deltaY / sideLength };

    FixedUnit currentX{ fixedA.at(0) };
    FixedUnit currentY{ fixedA.at(1) };
    for (uint16_t i{ 0 }; i <= static_cast<uint16_t>(sideLength); ++i)
    {
        DrawPixel(static_cast<uint16_t>(currentX), static_cast<uint16_t>(currentY), color);
        currentX += xIncrement;
        currentY += yIncrement;
    }
}

void Renderer::DrawTriangle(Vector2 a, Vector2 b, Vector2 c, uint32_t color)
{
    // First, convert points to fixed-point values such that we have consistent precision
    // between mathematical operations. Otherwise, differences in precision can appear as visual
    // artifacts between triangle edges.
    Vector2 fixedA{ FixedUnit{ a.x }, FixedUnit{ a.y } };
    Vector2 fixedB{ FixedUnit{ b.x }, FixedUnit{ b.y } };
    Vector2 fixedC{ FixedUnit{ c.x }, FixedUnit{ c.y } };

    // Confirm vertices are provided in counter-clockwise order
    if (TriangleDeterminant(fixedA, fixedB, fixedC) <= c_zero)
    {
        // SPDLOG_WARN("Invalid winding order of triangle vertices ({},{}), ({},{}), ({},{})",
        //     a.x, a.y, b.x, b.y, c.x, c.y);
        return;
    }

    // Simple rasterization - test every pixel of the rectangular boundary
    // surrounding the triangle and fill the points "inside" the triangle edges
    const auto xMin = static_cast<uint32_t>(fpm::floor(std::min(fixedA.x,
        std::min(fixedB.x, fixedC.x))));
    const auto yMin = static_cast<uint32_t>(fpm::floor(std::min(fixedA.y,
        std::min(fixedB.y, fixedC.y))));
    const auto xMax = static_cast<uint32_t>(fpm::ceil(std::max(fixedA.x,
        std::max(fixedB.x, fixedC.x))));
    const auto yMax = static_cast<uint32_t>(fpm::ceil(std::max(fixedA.y,
        std::max(fixedB.y, fixedC.y))));
    for (auto y{ yMin }; y <= yMax; ++y)
    {
        for (auto x{ xMin }; x <= xMax; ++x)
        {
            Vector2 point{ FixedUnit{ x + 0.5f }, FixedUnit{ y + 0.5f } };

            Vector3 edges{
                TriangleDeterminant(fixedB, fixedC, point),
                TriangleDeterminant(fixedC, fixedA, point),
                TriangleDeterminant(fixedA, fixedB, point)
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
                DrawPixel(static_cast<uint16_t>(x), static_cast<uint16_t>(y), color);
            }
        }
    }
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
#if TRUE
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
#if FALSE
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

        DrawTriangle(screenSpaceCoordinates.at(0), screenSpaceCoordinates.at(1), screenSpaceCoordinates.at(2), 0xFF00FFFF);
    }
#endif
}
