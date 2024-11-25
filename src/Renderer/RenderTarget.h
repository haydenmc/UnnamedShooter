#pragma once

struct RenderTarget
{
    RenderTarget(uint16_t width, uint16_t height);
    uint32_t const& PixelAt(uint16_t x, uint16_t y);
    void ClearBuffer();
    void ClearBuffer(uint32_t color);
    void DrawPixel(uint16_t x, uint16_t y, uint32_t color);
    void DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2,
        uint16_t y2, uint32_t color);
    void DrawLine(Vector2 a, Vector2 b, uint32_t color);
    void DrawTriangle(Vector2 a, Vector2 b, Vector2 c, uint32_t color);

    uint16_t const Width;
    uint16_t const Height;
    std::vector<uint32_t> Buffer;
};