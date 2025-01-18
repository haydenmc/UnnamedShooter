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
}

RenderTarget::RenderTarget(uint16_t width, uint16_t height): Width{ width }, Height{ height },
    Buffer((Width * Height), c_defaultBackgroundColor)
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

// Based off of the great article here on reasonably fast + accurate triangle rasterization:
// https://kristoffer-dyrkorn.github.io/triangle-rasterizer/
void RenderTarget::DrawTriangle(Vector2 a, Vector2 b, Vector2 c, uint32_t color)
{
    // Confirm vertices are provided in counter-clockwise order
    if (TriangleDeterminant(a, b, c) <= FixedUnit{ 0 })
    {
        return;
    }

    // Simple rasterization - test every pixel of the rectangular boundary
    // surrounding the triangle and fill the points "inside" the triangle edges
    const auto xMin = static_cast<uint16_t>(fpm::floor(std::min(a.x, std::min(b.x, c.x))));
    const auto yMin = static_cast<uint16_t>(fpm::floor(std::min(a.y, std::min(b.y, c.y))));
    const auto xMax = static_cast<uint16_t>(fpm::ceil(std::max(a.x, std::max(b.x, c.x))));
    const auto yMax = static_cast<uint16_t>(fpm::ceil(std::max(a.y, std::max(b.y, c.y))));

    // Begin with pre-calculating the edge distances at the top-left point.
    // The rest of the pixel values can be incrementally calculated from here.
    Vector2 topLeft{ (xMin + FixedUnit{ 0.5 }), (yMin + FixedUnit{ 0.5 }) };
    Vector3 wLeft{
        TriangleDeterminant(b, c, topLeft),
        TriangleDeterminant(c, a, topLeft),
        TriangleDeterminant(a, b, topLeft) };
    if (IsTriangleEdgeLeftOrTop(b, c))
    {
        wLeft.x -= std::numeric_limits<FixedUnit>::epsilon();
    }
    if (IsTriangleEdgeLeftOrTop(c, a))
    {
        wLeft.y -= std::numeric_limits<FixedUnit>::epsilon();
    }
    if (IsTriangleEdgeLeftOrTop(a, b))
    {
        wLeft.z -= std::numeric_limits<FixedUnit>::epsilon();
    }
    // Calculate determinant difference when moving across X axis and Y axis
    Vector3 dwdx{ (b.y - c.y), (c.y - a.y), (a.y - b.y) };
    Vector3 dwdy{ (b.x - c.x), (c.x - a.x), (a.x - b.x) };
    
    for (auto y{ yMin }; y <= yMax; ++y)
    {
        Vector3 horizontalW{ wLeft };
        for (auto x{ xMin }; x <= xMax; ++x)
        {
            if ((horizontalW.x >= FixedUnit{ 0 }) && (horizontalW.y >= FixedUnit{ 0 }) &&
                (horizontalW.z >= FixedUnit{ 0 }))
            {
                DrawPixel(static_cast<uint16_t>(x), static_cast<uint16_t>(y), color);
            } 
            Vector2 point{ FixedUnit{ x + 0.5f }, FixedUnit{ y + 0.5f } };

            horizontalW = (horizontalW - dwdx);
        }
        wLeft = (wLeft + dwdy);
    }
}