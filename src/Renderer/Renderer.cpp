#include <pch.h>
#include "Configuration.h"
#include "../Mesh/Mesh.h"
#include "Renderer.h"

namespace
{
    constexpr float c_defaultFovYRads{ 3.14159265f / 2.0f }; // TODO: This really belongs in the Camera
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

    Eigen::Matrix4f CreatePerspectiveMatrix(float fovYRads, float width, float height,
        float nearPlane, float farPlane)
    {
        return game::PerspectiveProjectionTransformMatrix(fovYRads, (height / width), nearPlane,
            farPlane);
    }

    std::unordered_map<FrustumPlaneKind, game::Plane> CreateFrustumPlanes(float fovX, float fovY,
        float zNear, float zFar)
    {
        std::unordered_map<FrustumPlaneKind, game::Plane> result;

        float xCosHalfFov{ cosf(fovX / 2.0f) };
        float xSinHalfFov{ sinf(fovX / 2.0f) };
        float yCosHalfFov{ cosf(fovY / 2.0f) };
        float ySinHalfFov{ sinf(fovY / 2.0f) };
        Eigen::Vector3f origin{ 0.0f, 0.0f, 0.0f };

        result[FrustumPlaneKind::LeftFrustumPlane] = game::Plane{
            .Point = origin,
            .Normal = Eigen::Vector3f{ xCosHalfFov, 0.0f, xSinHalfFov },
        };
        result[FrustumPlaneKind::RightFrustumPlane] = game::Plane{
            .Point = origin,
            .Normal = Eigen::Vector3f{ -xCosHalfFov, 0.0f, xSinHalfFov },
        };
        result[FrustumPlaneKind::TopFrustumPlane] = game::Plane{
            .Point = origin,
            .Normal = Eigen::Vector3f{ 0.0f, -yCosHalfFov, ySinHalfFov },
        };
        result[FrustumPlaneKind::BottomFrustumPlane] = game::Plane{
            .Point = origin,
            .Normal = Eigen::Vector3f{ 0.0f, yCosHalfFov, ySinHalfFov },
        };
        result[FrustumPlaneKind::NearFrustumPlane] = game::Plane{
            .Point = Eigen::Vector3f{ 0.0f, 0.0f, zNear },
            .Normal = Eigen::Vector3f{ 0.0f, 0.0f, 1.0f },
        };
        result[FrustumPlaneKind::FarFrustumPlane] = game::Plane{
            .Point = Eigen::Vector3f{ 0.0f, 0.0f, zFar },
            .Normal = Eigen::Vector3f{ 0.0f, 0.0f, -1.0f },
        };

        return result;
    }

    Eigen::Vector3f GetTriangleNormal(std::array<Eigen::Vector4f, 3> const& vertices)
    {
        auto a{ vertices.at(0).head<3>() };
        auto b{ vertices.at(1).head<3>() };
        auto c{ vertices.at(2).head<3>() };
        Eigen::Vector3f ab{ b - a };
        ab.normalize();
        Eigen::Vector3f ac{ c - a };
        ac.normalize();
        return ab.cross(ac).normalized();
    }

    float GetFovX(uint16_t width, uint16_t height, float fovY)
    {
        float aspectX = (width / static_cast<float>(height));
        return (atanf(tanf(fovY / 2.0f) * aspectX) * 2.0f);
    }

    game::Polygon ClipPolygonAgainstPlane(game::Polygon const& polygon, game::Plane const& plane)
    {
        if (polygon.Vertices.size() == 0)
        {
            return polygon;
        }

        Eigen::Vector3f const& planePoint{ plane.Point };
        Eigen::Vector3f const& planeNormal{ plane.Normal };

        game::Polygon result;
        size_t currentVertexIndex = 0;
        size_t currentTextureCoordIndex = 0;
        size_t previousVertexIndex = polygon.Vertices.size() - 1;
        size_t previousTextureCoordIndex = polygon.TextureCoordinates.size() - 1;

        float previousDot{
            (polygon.Vertices.at(previousVertexIndex) - planePoint).dot(planeNormal) };

        while (currentVertexIndex != polygon.Vertices.size())
        {
            const auto& currentVertex{ polygon.Vertices.at(currentVertexIndex) };
            const auto& currentTextureCoord{
                polygon.TextureCoordinates.at(currentTextureCoordIndex) };
            const auto& previousVertex{ polygon.Vertices.at(previousVertexIndex) };
            const auto& previousTextureCoord{
                polygon.TextureCoordinates.at(previousTextureCoordIndex) };
            float currentDot{ (currentVertex - planePoint).dot(planeNormal) };

            // Signs have changed between last dot and current dot, indicating
            // the line between the previous and current vertices has crossed
            // the plane boundary
            if (currentDot * previousDot < 0.0f)
            {
                // Split the polygon at the intersection point of the line and
                // the plane
                float t{ previousDot / (previousDot - currentDot) };
                Eigen::Vector3f intersectionPoint{
                    game::Lerp(previousVertex.x(), currentVertex.x(), t),
                    game::Lerp(previousVertex.y(), currentVertex.y(), t),
                    game::Lerp(previousVertex.z(), currentVertex.z(), t),
                };
                Eigen::Vector2f interpolatedTextureCoord{
                    game::Lerp(previousTextureCoord.x(), currentTextureCoord.x(), t),
                    game::Lerp(previousTextureCoord.y(), currentTextureCoord.y(), t),
                };
                result.Vertices.push_back(intersectionPoint);
                result.TextureCoordinates.push_back(interpolatedTextureCoord);
            }

            if (currentDot > 0.0f)
            {
                // Current vertex is inside the plane
                result.Vertices.push_back(currentVertex);
                result.TextureCoordinates.push_back(currentTextureCoord);
            }

            previousDot = currentDot;
            previousVertexIndex = currentVertexIndex;
            ++currentVertexIndex;
            previousTextureCoordIndex = currentTextureCoordIndex;
            ++currentTextureCoordIndex;
        }

        return result;
    }

    std::vector<game::Triangle> TrianglesFromPolygon(game::Polygon const& polygon)
    {
        std::vector<game::Triangle> result;
        for (size_t i{ 0 }; i < (polygon.Vertices.size() - 2); ++i)
        {
            const size_t firstIndex{ 0 };
            const size_t secondIndex{ i + 1 };
            const size_t thirdIndex{ i + 2 };
            game::Triangle t{
                .Vertices = {
                    polygon.Vertices.at(firstIndex),
                    polygon.Vertices.at(secondIndex),
                    polygon.Vertices.at(thirdIndex),
                },
                .TextureCoordinates = {
                    polygon.TextureCoordinates.at(firstIndex),
                    polygon.TextureCoordinates.at(secondIndex),
                    polygon.TextureCoordinates.at(thirdIndex),
                }
            };
            result.push_back(t);
        }
        return result;
    }
}

namespace game
{
Renderer::Renderer(std::shared_ptr<SDL_Window> window, const VideoConfiguration& resolution) :
    m_window{ window }, m_resolution{ resolution }, m_renderer{ CreateRenderer(m_window.get()) },
    m_frameBuffer{ m_resolution.Width, m_resolution.Height },
    m_frameBufferTexture{ CreateFrameBufferTexture(m_renderer.get(), m_resolution) },
    m_projectionMatrix{ CreatePerspectiveMatrix(c_defaultFovYRads, m_resolution.Width,
        m_resolution.Height, c_nearPlane, c_farPlane) },
    m_frustumPlanes{ CreateFrustumPlanes(GetFovX(m_resolution.Width, m_resolution.Height,
        c_defaultFovYRads), c_defaultFovYRads, c_nearPlane, c_farPlane) }
{}

void Renderer::Render(SimulationState const& simulationState)
{
    m_frameBuffer.ClearBuffers();
    DrawScene(&simulationState.Camera, &simulationState.RootWorldEntity);
    for (auto const& overlay : m_overlays)
    {
        overlay->Paint(&m_frameBuffer, simulationState);
    }
    CheckSdlReturn(SDL_UpdateTexture(m_frameBufferTexture.get(), nullptr,
        m_frameBuffer.Buffer.data(), (m_resolution.Width * sizeof(uint32_t))));
    CheckSdlReturn(SDL_RenderCopy(m_renderer.get(), m_frameBufferTexture.get(), nullptr, nullptr));
    SDL_RenderPresent(m_renderer.get());
}

void Renderer::AddOverlay(std::shared_ptr<Overlay> overlay)
{
    m_overlays.push_back(overlay);
}

void Renderer::DrawScene(CameraEntity const *cameraEntity, Entity const *sceneEntity)
{
    // Calculate view/camera matrix
    // TODO: Respect camera rotation
    Eigen::Vector4f cameraDirection{ Rotation(cameraEntity->GetRotation()) * Eigen::Vector4f{ 0.0f, 0.0f, 1.0f, 1.0f } };
    Eigen::Vector3f camdir3{ cameraDirection.head<3>() };
    Eigen::Vector3f cameraTarget{ cameraEntity->GetPosition() + camdir3 };
    Eigen::Matrix4f viewMatrix{ game::LookAt(cameraEntity->GetPosition(), cameraTarget,
        Eigen::Vector3f{ 0.0f, 1.0f, 0.0f }) };
    DrawEntityTreeMeshes(viewMatrix, sceneEntity);
}

void Renderer::DrawEntityTreeMeshes(Eigen::Matrix4f const& viewMatrix, Entity const* rootEntity)
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

void Renderer::DrawEntityMesh(Eigen::Matrix4f const& viewMatrix, Entity const* entity, Mesh const *mesh)
{
    // First apply entity transformations
    auto entityTranslate{ Translation(entity->GetPosition()) };
    auto entityRotate { Rotation(entity->GetRotation()) };

    // Textured
    for (const auto& face : mesh->Faces)
    {
        // Prepare vertices to be transformed via matrix multiplication
        const auto& pointA{ mesh->Vertices.at(face.MeshVertexIndices.at(0)) };
        const auto& pointB{ mesh->Vertices.at(face.MeshVertexIndices.at(1)) };
        const auto& pointC{ mesh->Vertices.at(face.MeshVertexIndices.at(2)) };
        std::array<Eigen::Vector2f, 3> textureCoordinates{
            mesh->TextureCoordinates.at(face.MeshTextureCoordinateIndices.at(0)),
            mesh->TextureCoordinates.at(face.MeshTextureCoordinateIndices.at(1)),
            mesh->TextureCoordinates.at(face.MeshTextureCoordinateIndices.at(2)),
        };
        std::array<Eigen::Vector4f, 3> transformedVertices{
            Eigen::Vector4f{ pointA.x(), pointA.y(), pointA.z(), 1.0f },
            Eigen::Vector4f{ pointB.x(), pointB.y(), pointB.z(), 1.0f },
            Eigen::Vector4f{ pointC.x(), pointC.y(), pointC.z(), 1.0f },
        };

        // Transform from local space -> world space -> camera space
        for (auto& vertex : transformedVertices)
        {
            vertex = viewMatrix * entityTranslate * entityRotate * vertex;
        }

        // Determine if this face is not visible and should be culled
        auto faceNormal{ GetTriangleNormal(transformedVertices) };
        Eigen::Vector3f cameraRay{ Eigen::Vector3f{ 0.0f, 0.0f, 0.0f } -
            Eigen::Vector3f{ transformedVertices.at(0).x(), transformedVertices.at(0).y(),
                transformedVertices.at(0).z() } };
        if (faceNormal.dot(cameraRay) <= 0.0f)
        {
            continue;
        }

        // Clip the triangle to the camera frustum boundary
        Polygon polygon{
            .Vertices = {
                transformedVertices.at(0).head<3>(),
                transformedVertices.at(1).head<3>(),
                transformedVertices.at(2).head<3>(),
            },
            .TextureCoordinates = std::vector<Eigen::Vector2f>(textureCoordinates.begin(),
                textureCoordinates.end()),
        };
        for (const auto& planePair: m_frustumPlanes)
        {
            polygon = ClipPolygonAgainstPlane(polygon, planePair.second);
        }

        if (polygon.Vertices.size() < 3)
        {
            continue;
        }

        auto triangles{ TrianglesFromPolygon(polygon) };

        for (auto& triangle : triangles)
        {
            // Apply perspective projection
            std::array<Eigen::Vector4f, 3> projectedVertices;
            for (size_t i{ 0 }; i < triangle.Vertices.size(); ++i)
            {
                const auto& vertex{ triangle.Vertices.at(i) };
                Eigen::Vector4f v{ vertex.x(), vertex.y(), vertex.z(), 1.0f };
                projectedVertices.at(i) = m_projectionMatrix * v;
            }

            // Translate to screen space
            float halfWidth{ m_resolution.Width / 2.0f };
            float halfHeight{ m_resolution.Height / 2.0f };
            for (auto& vertex : projectedVertices)
            {
                vertex.x() = (vertex.x() / vertex.w()) * halfWidth + halfWidth;
                vertex.y() = (vertex.y() / vertex.w()) * halfHeight + halfHeight;
            }
#if TRUE
            m_frameBuffer.DrawTexturedTriangle(projectedVertices.at(0), projectedVertices.at(1),
            projectedVertices.at(2), triangle.TextureCoordinates.at(0),
            triangle.TextureCoordinates.at(1), triangle.TextureCoordinates.at(2),
            mesh->Texture.get());
#endif
#if TRUE
            // Draw wireframe
            m_frameBuffer.DrawLine(projectedVertices.at(0).head<2>(), projectedVertices.at(1).head<2>(), 0xFF00FF00);
            m_frameBuffer.DrawLine(projectedVertices.at(1).head<2>(), projectedVertices.at(2).head<2>(), 0xFF00FF00);
            m_frameBuffer.DrawLine(projectedVertices.at(2).head<2>(), projectedVertices.at(0).head<2>(), 0xFF00FF00);
#endif
        }
    }
}
}