#pragma once
#include "../Texture/PngTexture.h"

struct RenderTarget
{
    RenderTarget(uint16_t width, uint16_t height);
    uint32_t const& PixelAt(uint16_t x, uint16_t y);
    void ClearBuffer();
    void ClearBuffer(uint32_t color);
    void ClearZBuffer();
    void DrawPixel(uint16_t x, uint16_t y, uint32_t color);
    void DrawTexel(uint16_t x, uint16_t y, std::shared_ptr<PngTexture> texture,
        Vector4 vertA, Vector4 vertB, Vector4 vertC,
        Vector2 texA, Vector2 texB, Vector2 texC);
    void DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2,
        uint16_t y2, uint32_t color);
    void DrawLine(Vector2 a, Vector2 b, uint32_t color);
    void DrawTexturedTriangle(Vector4 vertA, Vector4 vertB, Vector4 vertC,
        Vector2 texA, Vector2 texB, Vector2 texC, std::shared_ptr<PngTexture> texture);

    uint16_t const Width;
    uint16_t const Height;
    std::vector<uint32_t> Buffer;
    std::vector<FixedUnit> ZBuffer;
};