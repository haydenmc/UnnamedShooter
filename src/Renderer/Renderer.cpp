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

    Matrix4x4 CreatePerspectiveMatrix(float fovYRads, float width, float height,
        float nearPlane, float farPlane)
    {
        return Matrix4x4::PerspectiveProjection(FixedUnit{ fovYRads }, FixedUnit{ height / width },
            FixedUnit{ nearPlane }, FixedUnit{ farPlane });
    }

    std::unordered_map<FrustumPlaneKind, Plane> CreateFrustumPlanes(FixedUnit fovX, FixedUnit fovY,
        FixedUnit zNear, FixedUnit zFar)
    {
        std::unordered_map<FrustumPlaneKind, Plane> result;

        FixedUnit xCosHalfFov{ fpm::cos(fovX / FixedUnit{ 2 }) };
        FixedUnit xSinHalfFov{ fpm::sin(fovX / FixedUnit{ 2 }) };
        FixedUnit yCosHalfFov{ fpm::cos(fovY / FixedUnit{ 2 }) };
        FixedUnit ySinHalfFov{ fpm::sin(fovY / FixedUnit{ 2 }) };
        Vector3 origin{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 0 } };

        result[FrustumPlaneKind::LeftFrustumPlane] = Plane{
            .Point = origin,
            .Normal = Vector3{
                .x = xCosHalfFov,
                .y = FixedUnit{ 0 },
                .z = xSinHalfFov,
            },
        };
        result[FrustumPlaneKind::RightFrustumPlane] = Plane{
            .Point = origin,
            .Normal = Vector3{
                .x = -xCosHalfFov,
                .y = FixedUnit{ 0 },
                .z = xSinHalfFov,
            },
        };
        result[FrustumPlaneKind::TopFrustumPlane] = Plane{
            .Point = origin,
            .Normal = Vector3{
                .x = FixedUnit{ 0 },
                .y = -yCosHalfFov,
                .z = ySinHalfFov,
            },
        };
        result[FrustumPlaneKind::BottomFrustumPlane] = Plane{
            .Point = origin,
            .Normal = Vector3{
                .x = FixedUnit{ 0 },
                .y = yCosHalfFov,
                .z = ySinHalfFov,
            },
        };
        result[FrustumPlaneKind::NearFrustumPlane] = Plane{
            .Point = Vector3{
                .x = FixedUnit{ 0 },
                .y = FixedUnit{ 0 },
                .z = zNear,
            },
            .Normal = Vector3{
                .x = FixedUnit{ 0 },
                .y = FixedUnit{ 0 },
                .z = FixedUnit{ 1 },
            },
        };
        result[FrustumPlaneKind::FarFrustumPlane] = Plane{
            .Point = Vector3{
                .x = FixedUnit{ 0 },
                .y = FixedUnit{ 0 },
                .z = zFar,
            },
            .Normal = Vector3{
                .x = FixedUnit{ 0 },
                .y = FixedUnit{ 0 },
                .z = FixedUnit{ -1 },
            },
        };

        return result;
    }

    Vector3 GetTriangleNormal(const std::array<Vector4, 3>& vertices)
    {
        Vector3 a{ vertices.at(0).x, vertices.at(0).y, vertices.at(0).z };
        Vector3 b{ vertices.at(1).x, vertices.at(1).y, vertices.at(1).z };
        Vector3 c{ vertices.at(2).x, vertices.at(2).y, vertices.at(2).z };
        Vector3 ab = (b - a).Normalize();
        Vector3 ac = (c - a).Normalize();
        return ab.Cross(ac).Normalize();
    }

    FixedUnit GetFovX(uint16_t width, uint16_t height, float fovY)
    {
        float aspectX = (width / static_cast<float>(height));
        return FixedUnit{ atanf(tanf(fovY / 2.0f) * aspectX) * 2.0f };
    }

    MyPolygon ClipPolygonAgainstPlane(const MyPolygon& polygon, const Plane& plane)
    {
        if (polygon.Vertices.size() == 0)
        {
            return polygon;
        }

        const Vector3& planePoint{ plane.Point };
        const Vector3& planeNormal{ plane.Normal };

        SPDLOG_DEBUG("Plane point: {}, {}, {}", static_cast<float>(planePoint.x), static_cast<float>(planePoint.y), static_cast<float>(planePoint.z));
        SPDLOG_DEBUG("Plane normal: {}, {}, {}", static_cast<float>(planeNormal.x), static_cast<float>(planeNormal.y), static_cast<float>(planeNormal.z));

        MyPolygon result;
        size_t currentVertexIndex = 0;
        size_t currentTextureCoordIndex = 0;
        size_t previousVertexIndex = polygon.Vertices.size() - 1;
        size_t previousTextureCoordIndex = polygon.TextureCoordinates.size() - 1;

        FixedUnit previousDot{ (polygon.Vertices.at(previousVertexIndex) - planePoint).Dot(planeNormal) };
        SPDLOG_DEBUG("previousDot: {}", static_cast<float>(previousDot));

        while (currentVertexIndex != polygon.Vertices.size())
        {
            const auto& currentVertex{ polygon.Vertices.at(currentVertexIndex) };
            const auto& currentTextureCoord{
                polygon.TextureCoordinates.at(currentTextureCoordIndex) };
            const auto& previousVertex{ polygon.Vertices.at(previousVertexIndex) };
            const auto& previousTextureCoord{
                polygon.TextureCoordinates.at(previousTextureCoordIndex) };

            SPDLOG_DEBUG("currentVertex: {}, {}, {}", static_cast<float>(currentVertex.x), static_cast<float>(currentVertex.y), static_cast<float>(currentVertex.z));
            FixedUnit currentDot{ (currentVertex - planePoint).Dot(planeNormal) };

            SPDLOG_DEBUG("currentDot: {}", static_cast<float>(currentDot));

            // Signs have changed between last dot and current dot, indicating
            // the line between the previous and current vertices has crossed
            // the plane boundary
            if (currentDot * previousDot < FixedUnit{ 0 })
            {
                SPDLOG_DEBUG("Boundary crossed between last and current");
                // Split the polygon at the intersection point of the line and
                // the plane
                FixedUnit t{ previousDot / (previousDot - currentDot) };
                Vector3 intersectionPoint{
                    .x = Lerp(previousVertex.x, currentVertex.x, t),
                    .y = Lerp(previousVertex.y, currentVertex.y, t),
                    .z = Lerp(previousVertex.z, currentVertex.z, t),
                };
                Vector2 interpolatedTextureCoord{
                    .x = Lerp(previousTextureCoord.x, currentTextureCoord.x, t),
                    .y = Lerp(previousTextureCoord.y, currentTextureCoord.y, t),
                };
                SPDLOG_DEBUG("intersectionPoint: {}, {}, {}", static_cast<float>(intersectionPoint.x), static_cast<float>(intersectionPoint.y), static_cast<float>(intersectionPoint.z));
                result.Vertices.push_back(intersectionPoint);
                result.TextureCoordinates.push_back(interpolatedTextureCoord);
            }

            if (currentDot > FixedUnit{ 0 })
            {
                SPDLOG_DEBUG("inside vertex: {}, {}, {}", static_cast<float>(currentVertex.x), static_cast<float>(currentVertex.y), static_cast<float>(currentVertex.z));
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

    std::vector<Triangle> TrianglesFromPolygon(const MyPolygon& polygon)
    {
        std::vector<Triangle> result;
        for (size_t i{ 0 }; i < (polygon.Vertices.size() - 2); ++i)
        {
            const size_t firstIndex{ 0 };
            const size_t secondIndex{ i + 1 };
            const size_t thirdIndex{ i + 2 };
            Triangle t{
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

Renderer::Renderer(std::shared_ptr<SDL_Window> window, const VideoConfiguration& resolution) :
    m_window{ window }, m_resolution{ resolution }, m_renderer{ CreateRenderer(m_window.get()) },
    m_frameBuffer{ m_resolution.Width, m_resolution.Height },
    m_frameBufferTexture{ CreateFrameBufferTexture(m_renderer.get(), m_resolution) },
    m_projectionMatrix{ CreatePerspectiveMatrix(c_defaultFovYRads, m_resolution.Width,
        m_resolution.Height, c_nearPlane, c_farPlane) },
    m_frustumPlanes{ CreateFrustumPlanes(GetFovX(m_resolution.Width, m_resolution.Height,
        c_defaultFovYRads), FixedUnit{ c_defaultFovYRads }, FixedUnit{ c_nearPlane },
        FixedUnit{ c_farPlane }) }
{}

void Renderer::Render(SimulationState const& simulationState)
{
    m_frameBuffer.ClearBuffers();
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

    // Textured
#if TRUE
    size_t faceNum{ 0 };
    for (const auto& face : mesh->Faces)
    {
        SPDLOG_DEBUG("Face {}", faceNum++);
        // Prepare vertices to be transformed via matrix multiplication
        const auto& pointA{ mesh->Vertices.at(face.MeshVertexIndices.at(0)) };
        const auto& pointB{ mesh->Vertices.at(face.MeshVertexIndices.at(1)) };
        const auto& pointC{ mesh->Vertices.at(face.MeshVertexIndices.at(2)) };
        std::array<Vector2, 3> textureCoordinates{
            mesh->TextureCoordinates.at(face.MeshTextureCoordinateIndices.at(0)),
            mesh->TextureCoordinates.at(face.MeshTextureCoordinateIndices.at(1)),
            mesh->TextureCoordinates.at(face.MeshTextureCoordinateIndices.at(2)),
        };
        std::array<Vector4, 3> transformedVertices{
            Vector4{ pointA.x, pointA.y, pointA.z, FixedUnit{ 1 } },
            Vector4{ pointB.x, pointB.y, pointB.z, FixedUnit{ 1 } },
            Vector4{ pointC.x, pointC.y, pointC.z, FixedUnit{ 1 } },
        };

        // Transform from local space -> world space -> camera space
        for (auto& vertex : transformedVertices)
        {
            vertex = viewMatrix * entityTranslate * entityRotate * vertex;
        }

        // Determine if this face is not visible and should be culled
        auto faceNormal{ GetTriangleNormal(transformedVertices) };
        auto cameraRay{ Vector3{ FixedUnit{ 0 }, FixedUnit{ 0 }, FixedUnit{ 0 } } -
            Vector3{ transformedVertices.at(0).x, transformedVertices.at(0).y,
                transformedVertices.at(0).z } };
        if (faceNormal.Dot(cameraRay) <= FixedUnit{ 0 })
        {
            continue;
        }

        // Clip the triangle to the camera frustum boundary
        MyPolygon polygon{
            .Vertices = {
                static_cast<Vector3>(transformedVertices.at(0)),
                static_cast<Vector3>(transformedVertices.at(1)),
                static_cast<Vector3>(transformedVertices.at(2)),
            },
            .TextureCoordinates = std::vector<Vector2>(textureCoordinates.begin(),
                textureCoordinates.end()),
        };
        for (const auto& planePair: m_frustumPlanes)
        {
            SPDLOG_DEBUG("Clipping against plane {}", static_cast<int>(planePair.first));
            polygon = ClipPolygonAgainstPlane(polygon, planePair.second);
        }

        if (polygon.Vertices.size() < 3)
        {
            SPDLOG_DEBUG("Polygon has {} vertices", polygon.Vertices.size());
            continue;
        }

        auto triangles{ TrianglesFromPolygon(polygon) };

        for (auto& triangle : triangles)
        {
            // Apply perspective projection
            std::array<Vector4, 3> projectedVertices;
            for (size_t i{ 0 }; i < triangle.Vertices.size(); ++i)
            {
                const auto& vertex{ triangle.Vertices.at(i) };
                Vector4 v{ vertex.x, vertex.y, vertex.z, FixedUnit{ 1 } };
                projectedVertices.at(i) = m_projectionMatrix * v;
            }

            // Translate to screen space
            FixedUnit halfWidth{ m_resolution.Width / 2.0f };
            FixedUnit halfHeight{ m_resolution.Height / 2.0f };
            for (auto& vertex : projectedVertices)
            {
                vertex.x = (vertex.x / vertex.w) * halfWidth + halfWidth;
                vertex.y = (vertex.y / vertex.w) * halfHeight + halfHeight;
            }

            m_frameBuffer.DrawTexturedTriangle(projectedVertices.at(0), projectedVertices.at(1),
                projectedVertices.at(2), triangle.TextureCoordinates.at(0),
                triangle.TextureCoordinates.at(1), triangle.TextureCoordinates.at(2),
                mesh->Texture);
        }
    }
#endif
}


