#pragma once

struct PngTexture
{
    static std::shared_ptr<PngTexture> FromPngFile(const std::filesystem::path& file);
    const uint16_t& Width();
    const uint16_t& Height();
    const uint32_t& ColorAt(uint16_t x, uint16_t y);
private:
    uint16_t m_width;
    uint16_t m_height;
    std::vector<uint32_t> m_pixels;
    PngTexture(const std::filesystem::path& file);
};