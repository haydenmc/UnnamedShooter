#pragma once
#include "../Texture/PngTexture.h"

namespace game
{
struct RenderTarget
{
    RenderTarget(uint16_t width, uint16_t height);
    uint32_t const& PixelAt(uint16_t x, uint16_t y);
    void ClearBuffers();
    void ClearPixelBuffer(uint32_t color);
    void ClearZBuffer();
    void DrawPixel(uint16_t x, uint16_t y, uint32_t color);
    void DrawTexel(uint16_t x, uint16_t y, PngTexture* texture,
        Eigen::Vector4f const& vertA, Eigen::Vector4f const& vertB, Eigen::Vector4f const& vertC,
        Eigen::Vector2f const& texA, Eigen::Vector2f const& texB, Eigen::Vector2f const& texC);
    void DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2,
        uint16_t y2, uint32_t color);
    void DrawLine(Eigen::Vector2f const& inA, Eigen::Vector2f const& inB, uint32_t color);
    void DrawShadedTriangle(Eigen::Vector4f const& vertA, Eigen::Vector4f const& vertB,
        Eigen::Vector4f const& vertC, uint32_t color);
    void DrawTexturedTriangle(Eigen::Vector4f const& vertA, Eigen::Vector4f const& vertB,
        Eigen::Vector4f const& vertC, Eigen::Vector2f const& texA, Eigen::Vector2f const& texB,
        Eigen::Vector2f const& texC, PngTexture* texture);

    uint16_t const Width;
    uint16_t const Height;
    std::vector<uint32_t> Buffer;
    std::vector<float> ZBuffer;
};
}