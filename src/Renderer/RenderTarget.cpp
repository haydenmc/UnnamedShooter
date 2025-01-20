#include <pch.h>
#include "RenderTarget.h"

namespace
{
    constexpr uint32_t c_defaultBackgroundColor{ 0xFF000000 };

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

    Vector3 BarycentricWeights(Vector2 vertA, Vector2 vertB, Vector2 vertC, Vector2 point)
    {
        Vector2 ac{ vertC - vertA };
        Vector2 ab{ vertB - vertA };
        Vector2 ap{ point - vertA };
        Vector2 pc{ vertC - point };
        Vector2 pb{ vertB - point };

        FixedUnit parallelogramAreaAbc{ ac.Cross(ab) };
        FixedUnit alpha{ pc.Cross(pb) / parallelogramAreaAbc };
        FixedUnit beta{ ac.Cross(ap) / parallelogramAreaAbc };
        FixedUnit gamma{ FixedUnit{ 1.0f } - alpha - beta };

        return Vector3{ alpha, beta, gamma };
    }
}

RenderTarget::RenderTarget(uint16_t width, uint16_t height): Width{ width }, Height{ height },
    Buffer((Width * Height), c_defaultBackgroundColor), ZBuffer((Width * Height), FixedUnit{ 1.0f })
{ }

uint32_t const &RenderTarget::PixelAt(uint16_t x, uint16_t y)
{
    return Buffer.at((Width * y) + x);
}

void RenderTarget::ClearBuffer()
{
    ClearBuffer(c_defaultBackgroundColor);
}

void RenderTarget::ClearBuffer(uint32_t color)
{
    std::fill(Buffer.begin(), Buffer.end(), color);
}

void RenderTarget::DrawPixel(uint16_t x, uint16_t y, uint32_t color)
{
    Buffer.at((Width * y) + x) = color;
}

void RenderTarget::DrawTexel(uint16_t x, uint16_t y, std::shared_ptr<PngTexture> texture,
    Vector4 vertA, Vector4 vertB, Vector4 vertC, Vector2 texA, Vector2 texB, Vector2 texC)
{
    Vector3 barycentricWeights{ BarycentricWeights(static_cast<Vector2>(vertA),
        static_cast<Vector2>(vertB), static_cast<Vector2>(vertC),
        Vector2{ FixedUnit{ x }, FixedUnit{ y } }) };
    FixedUnit& alpha{ barycentricWeights.x };
    FixedUnit& beta{ barycentricWeights.y };
    FixedUnit& gamma{ barycentricWeights.z };

    texA.y = 1 - texA.y;
    texB.y = 1 - texB.y;
    texC.y = 1 - texC.y;

    FixedUnit interpolatedU{ (texA.x / vertA.w) * alpha +
        (texB.x / vertB.w) * beta + (texC.x / vertC.w) * gamma };
    FixedUnit interpolatedV{ (texA.y / vertA.w) * alpha +
        (texB.y / vertB.w) * beta + (texC.y / vertC.w) * gamma };
    FixedUnit interpolatedReciprocalW{ (FixedUnit{ 1 } / vertA.w) * alpha +
        (FixedUnit{ 1 } / vertB.w) * beta +
        (FixedUnit{ 1 } / vertC.w) * gamma };
    interpolatedU /= interpolatedReciprocalW;
    interpolatedV /= interpolatedReciprocalW;

    // Intelligently clamp with wraparound to [0, 1]
    // (apparently some OBJ files use UV values > 1 to 'wrap around')
    interpolatedU = interpolatedU < FixedUnit{ 0.0f } ? FixedUnit{ 0.0f } :
        (interpolatedU > FixedUnit{ 1.0f } ?
            (interpolatedU - fpm::floor(interpolatedU)) : interpolatedU);
    interpolatedV = interpolatedV < FixedUnit{ 0.0f } ? FixedUnit{ 0.0f } :
        (interpolatedV > FixedUnit{ 1.0f } ?
            (interpolatedV - fpm::floor(interpolatedV)) : interpolatedV);

    uint16_t textureX{ std::clamp(static_cast<uint16_t>(interpolatedU * texture->Width()),
        uint16_t{ 0 }, static_cast<uint16_t>(texture->Width() - 1)) };
    uint16_t textureY{ std::clamp(static_cast<uint16_t>(interpolatedV * texture->Height()),
        uint16_t{ 0 }, static_cast<uint16_t>(texture->Height() - 1)) };
    uint32_t const& color{ texture->ColorAt(textureX, textureY) };

    DrawPixel(x, y, color);
}

void RenderTarget::ClearZBuffer()
{
    std::fill(ZBuffer.begin(), ZBuffer.end(), FixedUnit{ 1.0f });
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

void RenderTarget::DrawLine(Vector2 a, Vector2 b, uint32_t color)
{
    a.x += FixedUnit{ 0.5f };
    a.y += FixedUnit{ 0.5f };
    b.x += FixedUnit{ 0.5f };
    b.y += FixedUnit{ 0.5f };

    FixedUnit deltaX{ b.x - a.x };
    FixedUnit deltaY{ b.y - a.y };
    FixedUnit sideLength{ fpm::abs(deltaX) >= fpm::abs(deltaY) ?
        fpm::abs(deltaX) : fpm::abs(deltaY) };

    FixedUnit xIncrement{ deltaX / sideLength };
    FixedUnit yIncrement{ deltaY / sideLength };

    FixedUnit currentX{ a.x };
    FixedUnit currentY{ a.y };
    for (size_t i{ 0 }; i <= static_cast<size_t>(sideLength); ++i)
    {
        DrawPixel(static_cast<uint16_t>(currentX), static_cast<uint16_t>(currentY), color);
        currentX += xIncrement;
        currentY += yIncrement;
    }
}

void RenderTarget::DrawTexturedTriangle(Vector4 vertA, Vector4 vertB, Vector4 vertC,
    Vector2 texA, Vector2 texB, Vector2 texC, std::shared_ptr<PngTexture> texture)
{
    // Confirm vertices are provided in counter-clockwise order
    if (TriangleDeterminant(static_cast<Vector2>(vertA), static_cast<Vector2>(vertB),
        static_cast<Vector2>(vertC)) <= FixedUnit{ 0 })
    {
        return;
    }

    // Simple rasterization - test every pixel of the rectangular boundary
    // surrounding the triangle and fill the points "inside" the triangle edges
    const auto xMin = static_cast<uint16_t>(fpm::floor(std::min(vertA.x,
        std::min(vertB.x, vertC.x))));
    const auto yMin = static_cast<uint16_t>(fpm::floor(std::min(vertA.y,
        std::min(vertB.y, vertC.y))));
    const auto xMax = static_cast<uint16_t>(fpm::ceil(std::max(vertA.x,
        std::max(vertB.x, vertC.x))));
    const auto yMax = static_cast<uint16_t>(fpm::ceil(std::max(vertA.y,
        std::max(vertB.y, vertC.y))));

    // Begin with pre-calculating the edge distances at the top-left point.
    // The rest of the pixel values can be incrementally calculated from here.
    Vector2 topLeft{ (xMin + FixedUnit{ 0.5 }), (yMin + FixedUnit{ 0.5 }) };
    Vector3 wLeft{
        TriangleDeterminant(static_cast<Vector2>(vertB), static_cast<Vector2>(vertC), topLeft),
        TriangleDeterminant(static_cast<Vector2>(vertC), static_cast<Vector2>(vertA), topLeft),
        TriangleDeterminant(static_cast<Vector2>(vertA), static_cast<Vector2>(vertB), topLeft) };
    if (IsTriangleEdgeLeftOrTop(static_cast<Vector2>(vertB), static_cast<Vector2>(vertC)))
    {
        wLeft.x -= std::numeric_limits<FixedUnit>::epsilon();
    }
    if (IsTriangleEdgeLeftOrTop(static_cast<Vector2>(vertC), static_cast<Vector2>(vertA)))
    {
        wLeft.y -= std::numeric_limits<FixedUnit>::epsilon();
    }
    if (IsTriangleEdgeLeftOrTop(static_cast<Vector2>(vertA), static_cast<Vector2>(vertB)))
    {
        wLeft.z -= std::numeric_limits<FixedUnit>::epsilon();
    }
    // Calculate determinant difference when moving across X axis and Y axis
    Vector3 dwdx{ (vertB.y - vertC.y), (vertC.y - vertA.y), (vertA.y - vertB.y) };
    Vector3 dwdy{ (vertB.x - vertC.x), (vertC.x - vertA.x), (vertA.x - vertB.x) };
    
    for (auto y{ yMin }; y <= yMax; ++y)
    {
        Vector3 horizontalW{ wLeft };
        for (auto x{ xMin }; x <= xMax; ++x)
        {
            if ((horizontalW.x >= FixedUnit{ 0 }) && (horizontalW.y >= FixedUnit{ 0 }) &&
                (horizontalW.z >= FixedUnit{ 0 }))
            {
                DrawTexel(static_cast<uint16_t>(x), static_cast<uint16_t>(y), texture, vertA, vertB, vertC, texA, texB, texC);
                //DrawPixel(static_cast<uint16_t>(x), static_cast<uint16_t>(y), 0xFFFF0000);
            } 
            Vector2 point{ FixedUnit{ x + 0.5f }, FixedUnit{ y + 0.5f } };

            horizontalW = (horizontalW - dwdx);
        }
        wLeft = (wLeft + dwdy);
    }
}