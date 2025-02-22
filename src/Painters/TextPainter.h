#pragma once
#include "../Texture/PngTexture.h"

struct RenderTarget;

struct BitmapFontCharacterData
{
    uint32_t Id;
    uint16_t TextureX;
    uint16_t TextureY;
    uint16_t Width;
    uint16_t Height;
    uint16_t OffsetX;
    uint16_t OffsetY;
    uint16_t AdvanceX;
    // uint8_t Page; // Multi-texture fonts not supported
    // uint8_t Channel; // Non-monochrome fonts not supported
};

struct BitmapFontData
{
    // Info block
    uint16_t FontSize;
    std::string FontName;
    // Common block
    uint16_t LineHeight;
    uint16_t Base;
    std::string TextureFileName;
    std::unordered_map<uint32_t, BitmapFontCharacterData> Characters;
};

struct TextPainter
{
    static std::shared_ptr<TextPainter> FromBitmapFont(std::filesystem::path fontFile);
    void PaintText(RenderTarget* target, std::string const& text) const;

private:
    TextPainter(std::filesystem::path fontFile);
    BitmapFontData m_fontData;
    std::shared_ptr<PngTexture> m_fontTexture;
};