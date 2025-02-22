#include "pch.h"
#include "PngTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::shared_ptr<PngTexture> PngTexture::FromPngFile(const std::filesystem::path &file)
{
    return std::shared_ptr<PngTexture>(new PngTexture(file));
}

const uint16_t &PngTexture::Width()
{
    return m_width;
}

const uint16_t &PngTexture::Height()
{
    return m_height;
}

const uint32_t &PngTexture::ColorAt(uint16_t x, uint16_t y)
{
    auto index = (y * m_width) + x;
    return m_pixels.at(index);
}

PngTexture::PngTexture(const std::filesystem::path& file)
{
    SPDLOG_INFO("Loading texture from PNG file '{}'...", file.string());
    int width;
    int height;
    int componentsPerPixel;
    unsigned char* data{ stbi_load(
        file.string().c_str(), &width, &height, &componentsPerPixel, 4) };
    if (data == nullptr)
    {
        SPDLOG_ERROR("Error loading PNG data for '{}'", file.string());
        throw std::invalid_argument("Error loading PNG data");
    }
    if (componentsPerPixel != 4)
    {
        SPDLOG_ERROR("PNG file '{}' is not 4 channels per pixel - cannot load", file.string());
        throw std::invalid_argument("PNG specified with invalid channels (4 expected)");
    }
    m_width = static_cast<uint16_t>(width);
    m_height = static_cast<uint16_t>(height);
    m_pixels.reserve(width * height);
    for (int i{ 0 }; i < height; ++i)
    {
        for (int j{ 0 }; j < width; ++j)
        {
            int index{ (i * componentsPerPixel * width) + (j * componentsPerPixel) };
            unsigned char red{   data[index] };
            unsigned char green{ data[index + 1] };
            unsigned char blue{  data[index + 2] };
            unsigned char alpha{ data[index + 3] }; 
            // Internal 32-bit pixel format is 0xAARRGGBB
            uint32_t color{ static_cast<uint32_t>(blue) |
                (static_cast<uint32_t>(green) << 8) |
                (static_cast<uint32_t>(red) << 16) |
                (static_cast<uint32_t>(alpha) << 24) };
            m_pixels.emplace_back(color);
        }
    }
    stbi_image_free(data);
    SPDLOG_INFO("Loaded texture '{}' @ {}x{} pixels", file.filename().string(), m_width,
        m_height);
}