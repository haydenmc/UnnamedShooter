#include <pch.h>
#include "RenderTarget.h"

namespace
{
    constexpr uint32_t c_defaultBackgroundColor{ 0xFF000000 };

    fpm::fixed_16_16 TriangleDeterminant(Eigen::Vector2<fpm::fixed_16_16> const& pointA,
        Eigen::Vector2<fpm::fixed_16_16> const& pointB,
        Eigen::Vector2<fpm::fixed_16_16> const& pointC)
    {
        Eigen::Vector2<fpm::fixed_16_16> const ab{ pointB - pointA };
        Eigen::Vector2<fpm::fixed_16_16> const ac{ pointC - pointA };
        fpm::fixed_16_16 det{((ab.y() * ac.x()) - (ab.x() * ac.y()))};
        SPDLOG_DEBUG("a: {},{} b: {},{} c: {},{}",
            static_cast<float>(pointA.x()), static_cast<float>(pointA.y()),
            static_cast<float>(pointB.x()), static_cast<float>(pointB.y()),
            static_cast<float>(pointC.x()), static_cast<float>(pointC.y()));
        SPDLOG_DEBUG("ab: {}, {} ac: {}, {}", static_cast<float>(ab.x()),
            static_cast<float>(ab.y()), static_cast<float>(ac.x()), static_cast<float>(ac.y()));
        SPDLOG_DEBUG("determinant: {}", static_cast<float>(det));

        return det;
    }

    bool IsTriangleEdgeLeftOrTop(Eigen::Vector2<fpm::fixed_16_16> const& pointA,
        Eigen::Vector2<fpm::fixed_16_16> const& pointB)
    {
        Eigen::Vector2<fpm::fixed_16_16> const edge{ pointB - pointA };
        const bool isLeftEdge{ edge.y() > fpm::fixed_16_16{ 0 } };
        const bool isTopEdge{ (edge.y() == fpm::fixed_16_16{ 0 }) &&
            (edge.x() < fpm::fixed_16_16{ 0 }) };
        return (isLeftEdge || isTopEdge);
    }

    Eigen::Vector3f BarycentricWeights(Eigen::Vector2f const& vertA, Eigen::Vector2f const& vertB,
        Eigen::Vector2f const& vertC, Eigen::Vector2f const& point)
    {
        Eigen::Vector2f ac{ vertC - vertA };
        Eigen::Vector2f ab{ vertB - vertA };
        Eigen::Vector2f ap{ point - vertA };
        Eigen::Vector2f pc{ vertC - point };
        Eigen::Vector2f pb{ vertB - point };

        float parallelogramAreaAbc{ game::CrossProduct2D(ac, ab) };
        float alpha{ game::CrossProduct2D(pc, pb) / parallelogramAreaAbc };
        float beta{ game::CrossProduct2D(ac, ap) / parallelogramAreaAbc };
        float gamma{ 1.0f - alpha - beta };

        return Eigen::Vector3f{ alpha, beta, gamma };
    }
}

namespace game
{
RenderTarget::RenderTarget(uint16_t width, uint16_t height): Width{ width }, Height{ height },
    Buffer((Width * Height), c_defaultBackgroundColor), ZBuffer((Width * Height), 1.0f)
{ }

uint32_t const &RenderTarget::PixelAt(uint16_t x, uint16_t y)
{
    return Buffer.at((Width * y) + x);
}

void RenderTarget::ClearBuffers()
{
    ClearPixelBuffer(c_defaultBackgroundColor);
    ClearZBuffer();
}

void RenderTarget::ClearPixelBuffer(uint32_t color)
{
    std::fill(Buffer.begin(), Buffer.end(), color);
}

void RenderTarget::DrawPixel(uint16_t x, uint16_t y, uint32_t color)
{
    Buffer.at((Width * y) + x) = color;
}

void RenderTarget::DrawTexel(uint16_t x, uint16_t y, std::shared_ptr<PngTexture> texture,
        Eigen::Vector4f const& vertA, Eigen::Vector4f const& vertB, Eigen::Vector4f const& vertC,
        Eigen::Vector2f const& texA, Eigen::Vector2f const& texB, Eigen::Vector2f const& texC)
{
    Eigen::Vector3f barycentricWeights{ BarycentricWeights(vertA.head<2>(), vertB.head<2>(),
        vertC.head<2>(), Eigen::Vector2f{ x, y }) };
    float const& alpha{ barycentricWeights.x() };
    float const& beta{ barycentricWeights.y() };
    float const& gamma{ barycentricWeights.z() };

    float interpolatedReciprocalW{
        (1.0f / vertA.w()) * alpha +
        (1.0f / vertB.w()) * beta +
        (1.0f / vertC.w()) * gamma };
    // Adjust 1/w so closer pixels have smaller values.
    float depthValue{ 1.0f - interpolatedReciprocalW };
    // Only draw pixel if it's "closer to screen" than previous pixel
    if (depthValue >= ZBuffer.at((Width * y) + x))
    {
        return;
    }

    float interpolatedU{ (texA.x() / vertA.w()) * alpha +
        (texB.x() / vertB.w()) * beta + (texC.x() / vertC.w()) * gamma };
    float interpolatedV{ ((1 - texA.y()) / vertA.w()) * alpha +
        ((1 - texB.y()) / vertB.w()) * beta + ((1 - texC.y()) / vertC.w()) * gamma };
    interpolatedU /= interpolatedReciprocalW;
    interpolatedV /= interpolatedReciprocalW;

    // Intelligently clamp with wraparound to [0, 1]
    // (apparently some OBJ files use UV values > 1 to 'wrap around')
    interpolatedU = interpolatedU < 0.0f ? 0.0f :
        (interpolatedU > 1.0f ?
            (interpolatedU - floorf(interpolatedU)) : interpolatedU);
    interpolatedV = interpolatedV < 0.0f ? 0.0f :
        (interpolatedV > 1.0f ?
            (interpolatedV - floorf(interpolatedV)) : interpolatedV);

    uint16_t textureX{ std::clamp(static_cast<uint16_t>(interpolatedU * texture->Width()),
        uint16_t{ 0 }, static_cast<uint16_t>(texture->Width() - 1)) };
    uint16_t textureY{ std::clamp(static_cast<uint16_t>(interpolatedV * texture->Height()),
        uint16_t{ 0 }, static_cast<uint16_t>(texture->Height() - 1)) };
    uint32_t const& color{ texture->ColorAt(textureX, textureY) };

    ZBuffer.at((Width * y) + x) = depthValue;
    DrawPixel(x, y, color);
}

void RenderTarget::ClearZBuffer()
{
    std::fill(ZBuffer.begin(), ZBuffer.end(), 1.0f);
}

void RenderTarget::DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
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

void RenderTarget::DrawLine(Eigen::Vector2f const& inA, Eigen::Vector2f const& inB, uint32_t color)
{
    Eigen::Vector2f a{ inA };
    Eigen::Vector2f b{ inB };
    a.x() += 0.5f;
    a.y() += 0.5f;
    b.x() += 0.5f;
    b.y() += 0.5f;

    float deltaX{ b.x() - a.x() };
    float deltaY{ b.y() - a.y() };
    float sideLength{ abs(deltaX) >= abs(deltaY) ?
        abs(deltaX) : abs(deltaY) };

    float xIncrement{ deltaX / sideLength };
    float yIncrement{ deltaY / sideLength };

    float currentX{ a.x() };
    float currentY{ a.y() };
    for (size_t i{ 0 }; i <= static_cast<size_t>(sideLength); ++i)
    {
        DrawPixel(static_cast<uint16_t>(currentX), static_cast<uint16_t>(currentY), color);
        currentX += xIncrement;
        currentY += yIncrement;
    }
}

void RenderTarget::DrawTexturedTriangle(Eigen::Vector4f const& vertA, Eigen::Vector4f const& vertB,
    Eigen::Vector4f const& vertC, Eigen::Vector2f const& texA, Eigen::Vector2f const& texB,
    Eigen::Vector2f const& texC, std::shared_ptr<PngTexture> texture)
{
    // First, convert vertices from floating point to fixed-point numbers to ensure we don't run
    // into precision issues when calculating ownership of triangle edges.
    Eigen::Vector2<fpm::fixed_16_16> const vertA2D{ fpm::fixed_16_16{ vertA.x() },
        fpm::fixed_16_16{ vertA.y() } };
    Eigen::Vector2<fpm::fixed_16_16> const vertB2D{ fpm::fixed_16_16{ vertB.x() },
        fpm::fixed_16_16{ vertB.y() } };
    Eigen::Vector2<fpm::fixed_16_16> const vertC2D{ fpm::fixed_16_16{ vertC.x() },
        fpm::fixed_16_16{ vertC.y() } };

    // Confirm vertices are provided in counter-clockwise order
    if (TriangleDeterminant(vertA2D, vertB2D, vertC2D) <= fpm::fixed_16_16{ 0 })
    {
        return;
    }

    // Simple rasterization - test every pixel of the rectangular boundary
    // surrounding the triangle and fill the points "inside" the triangle edges
    const auto xMin{ static_cast<uint16_t>(floor(std::min(vertA2D.x(),
        std::min(vertB2D.x(), vertC2D.x())))) };
    const auto yMin{ static_cast<uint16_t>(floor(std::min(vertA2D.y(),
        std::min(vertB2D.y(), vertC2D.y())))) };
    const auto xMax{ static_cast<uint16_t>(ceil(std::max(vertA2D.x(),
        std::max(vertB2D.x(), vertC2D.x())))) };
    const auto yMax{ static_cast<uint16_t>(ceil(std::max(vertA2D.y(),
        std::max(vertB2D.y(), vertC2D.y())))) };

    // Begin with pre-calculating the edge distances at the top-left point.
    // The rest of the pixel values can be incrementally calculated from here.
    Eigen::Vector2<fpm::fixed_16_16> topLeft{ fpm::fixed_16_16{ static_cast<float>(xMin) + 0.5f },
        fpm::fixed_16_16{ static_cast<float>(yMin) + 0.5f } };
    Eigen::Vector3<fpm::fixed_16_16> wLeft{
        TriangleDeterminant(vertB2D, vertC2D, topLeft),
        TriangleDeterminant(vertC2D, vertA2D, topLeft),
        TriangleDeterminant(vertA2D, vertB2D, topLeft) };
    if (IsTriangleEdgeLeftOrTop(vertB2D, vertC2D))
    {
        wLeft.x() -= std::numeric_limits<fpm::fixed_16_16>::epsilon();
    }
    if (IsTriangleEdgeLeftOrTop(vertC2D, vertA2D))
    {
        wLeft.y() -= std::numeric_limits<fpm::fixed_16_16>::epsilon();
    }
    if (IsTriangleEdgeLeftOrTop(vertA2D, vertB2D))
    {
        wLeft.z() -= std::numeric_limits<fpm::fixed_16_16>::epsilon();
    }

    // Calculate determinant difference when moving across X axis and Y axis
    Eigen::Vector3<fpm::fixed_16_16> dwdx{ (vertB2D.y() - vertC2D.y()),
        (vertC2D.y() - vertA2D.y()), (vertA2D.y() - vertB2D.y()) };
    Eigen::Vector3<fpm::fixed_16_16> dwdy{ (vertB2D.x() - vertC2D.x()),
        (vertC2D.x() - vertA2D.x()), (vertA2D.x() - vertB2D.x()) };
    
    for (auto y{ yMin }; y <= yMax; ++y)
    {
        Eigen::Vector3<fpm::fixed_16_16> horizontalW{ wLeft };
        for (auto x{ xMin }; x <= xMax; ++x)
        {
            if ((horizontalW.x() >= fpm::fixed_16_16{ 0 }) &&
                (horizontalW.y() >= fpm::fixed_16_16{ 0 }) &&
                (horizontalW.z() >= fpm::fixed_16_16{ 0 }))
            {
                DrawTexel(static_cast<uint16_t>(x), static_cast<uint16_t>(y), texture,
                    vertA, vertB, vertC, texA, texB, texC);
            }

            horizontalW = (horizontalW - dwdx);
        }
        wLeft = (wLeft + dwdy);
    }
}
}