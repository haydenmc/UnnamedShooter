#include <pch.h>
#include "TextPainter.h"
#include "../Renderer/RenderTarget.h"

namespace
{
    constexpr char c_fontFileSignature[]{ 'B', 'M', 'F' };
    constexpr uint8_t c_fontFileFormatVersion{ 3 };
    constexpr uint8_t c_infoBlockId{ 1 };
    constexpr size_t c_maxFontFileStringLength{ 512 };
    constexpr size_t c_numBytesPerChar{ 20 };

    enum class FontFileBlockIdKind : uint8_t
    {
        Unknown = 0,
        Info = 1,
        Common = 2,
        Pages = 3,
        Chars = 4,
        KerningPairs = 5,
    };

    void LoadBitmapFontFileInfoBlock(std::ifstream& file, BitmapFontData& fontData,
        uint32_t blockSize)
    {
        {
            int16_t fontSize;
            file.read(reinterpret_cast<char*>(&fontSize), sizeof(fontSize));
            // Font size is stored as a negative number for legacy reasons
            // see https://github.com/vladimirgamalyan/fontbm/issues/24
            fontData.FontSize = static_cast<uint16_t>(-fontSize);
        }
        // Skip bitField, charSet, stretchH, aa, padding, spacing, outline
        file.seekg(12, std::ios::cur);

        if ((blockSize - 14) > c_maxFontFileStringLength)
        {
            SPDLOG_ERROR("Invalid font file: font name exceeds length limit {}",
                c_maxFontFileStringLength);
            throw std::runtime_error("Font file page texture name exceeds length limit");
        }
        char buf[c_maxFontFileStringLength]{ 0 };
        file.read(reinterpret_cast<char*>(&buf), (blockSize - 14));
        fontData.FontName = std::string{ buf, blockSize };
    }

    void LoadBitmapFontFileCommonBlock(std::ifstream& file, BitmapFontData& fontData,
        uint32_t /* blockSize */)
    {
        file.read(reinterpret_cast<char*>(&fontData.LineHeight), sizeof(fontData.LineHeight));
        file.read(reinterpret_cast<char*>(&fontData.Base), sizeof(fontData.Base));
        // Skip scaleW and scaleH
        file.seekg(4, std::ios::cur);
        uint16_t pageCount{ 0 };
        file.read(reinterpret_cast<char*>(&pageCount), sizeof(pageCount));
        if (pageCount != 1)
        {
            SPDLOG_ERROR("Invalid font file: only single page count supported, file has {}",
                pageCount);
            throw std::runtime_error("Font file has unsupported page count");
        }
        // Skip the rest of the fields for now, since we don't use them
        file.seekg(5, std::ios::cur);
    }

    void LoadBitmapFontFilePagesBlock(std::ifstream& file, BitmapFontData& fontData,
        uint32_t blockSize)
    {
        if (blockSize > c_maxFontFileStringLength)
        {
            SPDLOG_ERROR("Invalid font file: page texture name exceeds length limit {}",
                c_maxFontFileStringLength);
            throw std::runtime_error("Font file page texture name exceeds length limit");
        }
        char buf[c_maxFontFileStringLength]{ 0 };
        file.read(reinterpret_cast<char*>(&buf), blockSize);
        fontData.TextureFileName = std::string{ buf, blockSize };
    }

    void LoadBitmapFontFileCharsBlock(std::ifstream& file, BitmapFontData& fontData,
        uint32_t blockSize)
    {
        size_t numChars{ blockSize / c_numBytesPerChar };
        for (size_t i{ 0 }; i < numChars; ++i)
        {
            BitmapFontCharacterData character;
            file.read(reinterpret_cast<char*>(&character.Id), sizeof(character.Id));
            file.read(reinterpret_cast<char*>(&character.TextureX), sizeof(character.TextureX));
            file.read(reinterpret_cast<char*>(&character.TextureY), sizeof(character.TextureY));
            file.read(reinterpret_cast<char*>(&character.Width), sizeof(character.Width));
            file.read(reinterpret_cast<char*>(&character.Height), sizeof(character.Height));
            file.read(reinterpret_cast<char*>(&character.OffsetX), sizeof(character.OffsetX));
            file.read(reinterpret_cast<char*>(&character.OffsetY), sizeof(character.OffsetY));
            file.read(reinterpret_cast<char*>(&character.AdvanceX), sizeof(character.AdvanceX));
            file.seekg(2, std::ios::cur); // Skip Page and Channel fields
            fontData.Characters.insert_or_assign(character.Id, character);
        }
    }

    void LoadBitmapFontFileBlock(std::ifstream& file, BitmapFontData& fontData)
    {
        uint32_t blockSize{ 0 };
        uint8_t blockId{ 0 };
        file.read(reinterpret_cast<char*>(&blockId), sizeof(blockId));
        file.read(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));
        switch (static_cast<FontFileBlockIdKind>(blockId))
        {
        case FontFileBlockIdKind::Info:
            LoadBitmapFontFileInfoBlock(file, fontData, blockSize);
            break;
        case FontFileBlockIdKind::Common:
            LoadBitmapFontFileCommonBlock(file, fontData, blockSize);
            break;
        case FontFileBlockIdKind::Pages:
            LoadBitmapFontFilePagesBlock(file, fontData, blockSize);
            break;
        case FontFileBlockIdKind::Chars:
            LoadBitmapFontFileCharsBlock(file, fontData, blockSize);
            break;
        case FontFileBlockIdKind::KerningPairs:
            file.seekg(blockSize, std::ios::cur); // Skip this block
            break;
        default:
            SPDLOG_ERROR("Invalid font file: unexpected info block id '{}'", blockId);
            throw std::runtime_error("Font file has incorrect info block id");
        }
    }

    BitmapFontData LoadBitmapFontData(std::filesystem::path const& fontFile)
    {
        SPDLOG_INFO("Loading font from file '{}'...", fontFile.filename().string());
        // Read the BMF "Bitmap Font Generator" font file as specified by
        // https://www.angelcode.com/products/bmfont/doc/file_format.html
        BitmapFontData fontData;
        std::ifstream file{ fontFile, std::ios::binary };
        if (!file)
        {
            SPDLOG_ERROR("Could not open font file '{}' for reading", fontFile.string());
            throw std::runtime_error("Couldn't open font file for reading");
        }
        {
            char fileSignature[sizeof(c_fontFileSignature)];
            file.read(&fileSignature[0], sizeof(c_fontFileSignature));
            if (strncmp(c_fontFileSignature, fileSignature, sizeof(c_fontFileSignature)) != 0)
            {
                SPDLOG_ERROR("Invalid font file '{}': incorrect signature", fontFile.string());
                throw std::runtime_error("Font file has incorrect signature");
            }
        }
        {
            uint8_t formatVersion;
            file.read(reinterpret_cast<char*>(&formatVersion), sizeof(formatVersion));
            if (c_fontFileFormatVersion != formatVersion)
            {
                SPDLOG_ERROR("Invalid font file '{}': incorrect format version", fontFile.string());
                throw std::runtime_error("Font file has incorrect format version");
            }
        }
        while (file.peek() != EOF)
        {
            LoadBitmapFontFileBlock(file, fontData);
        }
        SPDLOG_INFO("Loaded font '{}' with {} characters", fontData.FontName,
            fontData.Characters.size());
        return fontData;
    }

    std::shared_ptr<PngTexture> LoadFontTexture(BitmapFontData const& fontData)
    {
        return PngTexture::FromPngFile(fontData.TextureFileName);
    }
}

std::shared_ptr<TextPainter> TextPainter::FromBitmapFont(std::filesystem::path fontFile)
{
    return std::shared_ptr<TextPainter>(new TextPainter(fontFile));
}

void TextPainter::PaintText(RenderTarget* /*target*/, std::string const& /*text*/) const
{

}

TextPainter::TextPainter(std::filesystem::path fontFile) :
    m_fontData{ LoadBitmapFontData(fontFile) }, m_fontTexture{ LoadFontTexture(m_fontData) }
{ }