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
    void DrawTexel(uint16_t x, uint16_t y, std::shared_ptr<PngTexture> texture,
        Eigen::Vector4f vertA, Eigen::Vector4f vertB, Eigen::Vector4f vertC,
        Eigen::Vector2f texA, Eigen::Vector2f texB, Eigen::Vector2f texC);
    void DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2,
        uint16_t y2, uint32_t color);
    void DrawLine(Eigen::Vector2f a, Eigen::Vector2f b, uint32_t color);
    void DrawShadedTriangle(Eigen::Vector4f vertA, Eigen::Vector4f vertB, Eigen::Vector4f vertC,
        uint32_t color);
    void DrawTexturedTriangle(Eigen::Vector4f vertA, Eigen::Vector4f vertB, Eigen::Vector4f vertC,
        Eigen::Vector2f texA, Eigen::Vector2f texB, Eigen::Vector2f texC,
        std::shared_ptr<PngTexture> texture);

    uint16_t const Width;
    uint16_t const Height;
    std::vector<uint32_t> Buffer;
    std::vector<float> ZBuffer;
};
}