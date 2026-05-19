#include "spark.h"
#include "FontBlueprint.h"
#include "engine/bits/PowerOfTwo.h"
#include "engine/io/VFS.h"
#include "engine/reflect/Util.h"
#include "stb_truetype.h"
#include "engine/asset/font/Font.h"
#include "engine/asset/texture/Texture.h"
#include "engine/ui/FloatRect.h"
#include "engine/asset/builder/util/Bitmap.h"
#include "engine/threads/ParallelForEach.h"

namespace se::asset::builder
{
    constexpr int s_SDFOneEdge = 128;

    constexpr const char* s_FontMapChars =
            "!@#$%^&*()_+"
            "1234567890-="
            "qwertyuiop[]"
            "QWERTYUIOP{}"
            "asdfghjkl;'\\"
            "ASDFGHJKL:\"|"
            "zxcvbnm,./`"
            "ZXCVBNM<>?~/ ";

    std::regex FontBlueprint::GetFilePattern() const
    {
        return std::regex(".*.ttf");
    }

    std::vector<BuiltAsset> FontBlueprint::BuildAsset(const std::string& path,
                                                      const std::string& outputPath) const
    {
        std::vector<BuiltAsset> ret;
        Font font;

        stbtt_fontinfo info = { };
        void* fontData = nullptr;
        if (!LoadFont(info, &fontData, path))
        {
            return { };
        }

        auto lastSlashIt = outputPath.find_last_of('/');
        auto extensionIt = outputPath.find_last_of('.');
        std::string outputDir = outputPath.substr(0, lastSlashIt);
        std::string fontName = outputPath.substr(lastSlashIt + 1, extensionIt - 1 - lastSlashIt);

        font.m_Name = fontName;

        float scale = stbtt_ScaleForPixelHeight(&info, static_cast<float>(Scale));

        int ascent, descent, lineGap;
        stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

        font.m_Ascent = ascent * scale;
        font.m_Descent = descent * scale;
        font.m_LineGap = lineGap * scale;

        CreateSDFTexture(info, font, scale, ret, outputDir);

        for (int i = 1; i <= BitmapCutoffSize; ++i)
        {
            CreateBitmapTexture(info, font, i, ret, outputDir);
        }

        font.m_SourcePath = path;

        std::free(fontData);

        ret.push_back({ reflect::SerialiseType<Font>(&font), "" });

        return ret;
    }

    void FontBlueprint::CreateSDFTexture(stbtt_fontinfo& info,
        Font& font,
        float scale,
        std::vector<BuiltAsset>& ret,
        const std::string& outputPath)
    {
        size_t numChars = strlen(s_FontMapChars);
        auto boundingBoxes = CollectSortedSDFBoundingBoxes(info, scale);
        std::vector<ui::FloatRect> placedBoundingBoxes = { };
        placedBoundingBoxes.reserve(numChars);

        std::unordered_map<char, CharData> charDataMap;
        int imageWidth = 128;
        int imageHeight = 128;
        int interval = 1;
        for (size_t i = 0; i < numChars; ++i)
        {
            char c = s_FontMapChars[boundingBoxes[i].second];
            auto& charData = charDataMap[c];
            charData.rect = boundingBoxes[i].first;

            CollectCharMetrics(info, c, scale, font.GetAscent(Scale), charData);
            PackChar(charData.rect, placedBoundingBoxes, imageWidth, imageHeight, interval);
        }

        for (size_t i = 0; i < numChars; ++i)
        {
            char c = s_FontMapChars[boundingBoxes[i].second];
            const auto& FloatRect = placedBoundingBoxes[i];
            auto& charData = charDataMap[c];
            // calculate Uvs now that the image size is stable
            charData.uvTopLeft = math::Vec2(static_cast<float>(FloatRect.topLeft.x) / imageWidth,
                                            static_cast<float>(FloatRect.topLeft.y) / imageHeight);
            charData.uvBottomRight = math::Vec2((static_cast<float>(FloatRect.topLeft.x) + static_cast<float>(FloatRect.size.x)) / imageWidth,
                                                (static_cast<float>(FloatRect.topLeft.y) + static_cast<float>(FloatRect.size.y)) / imageHeight);
        }

        font.m_SDFCharData = charDataMap;
        memory::BinaryBlob sdfBitmap = GenerateSDFBitmap(info, imageWidth, imageHeight, scale,
                                                                       boundingBoxes, placedBoundingBoxes);

        std::shared_ptr<Texture> sdfTexture = Texture::FromRawData(imageWidth, imageHeight, sdfBitmap, texture::Format::R8, texture::Usage::Read);
        ret.push_back({ reflect::SerialiseType<Texture>(sdfTexture.get()), "_sdf" });
        font.m_SDFTexture = std::format("{}/{}_sdf.sass", outputPath, font.GetName());
    }

    void FontBlueprint::CreateBitmapTexture(stbtt_fontinfo& info,
        Font& font,
        int fontSize,
        std::vector<BuiltAsset>& ret,
        const std::string& outputPath)
    {
        float scale = stbtt_ScaleForPixelHeight(&info, static_cast<float>(fontSize));

        size_t numChars = strlen(s_FontMapChars);
        auto boundingBoxes = CollectSortedBitmapBoundingBoxes(info, scale);
        std::vector<ui::FloatRect> placedBoundingBoxes = { };
        placedBoundingBoxes.reserve(numChars);

        std::unordered_map<char, CharData> charDataMap;
        int imageWidth = 128;
        int imageHeight = 128;
        int interval = 1;
        for (size_t i = 0; i < numChars; ++i)
        {
            char c = s_FontMapChars[boundingBoxes[i].second];
            auto& charData = charDataMap[c];
            charData.rect = boundingBoxes[i].first;

            CollectCharMetrics(info, c, scale, font.GetAscent(fontSize), charData);
            PackChar(charData.rect, placedBoundingBoxes, imageWidth, imageHeight, interval);
        }

        for (size_t i = 0; i < numChars; ++i)
        {
            char c = s_FontMapChars[boundingBoxes[i].second];
            const auto& FloatRect = placedBoundingBoxes[i];
            auto& charData = charDataMap[c];
            // calculate Uvs now that the image size is stable
            charData.uvTopLeft = math::Vec2(FloatRect.topLeft.x / imageWidth,
                                            FloatRect.topLeft.y / imageHeight);
            charData.uvBottomRight = math::Vec2((FloatRect.topLeft.x + FloatRect.size.x) / imageWidth,
                                                (FloatRect.topLeft.y + FloatRect.size.y) / imageHeight);
        }

        font.m_BitmapCharData[fontSize] = charDataMap;
        memory::BinaryBlob bitmap = GenerateBitmap(info, imageWidth, imageHeight, scale,
                                                                       boundingBoxes, placedBoundingBoxes);

        std::shared_ptr<Texture> texture = Texture::FromRawData(imageWidth, imageHeight, bitmap, texture::Format::R8, texture::Usage::Read);
        ret.push_back({ reflect::SerialiseType<Texture>(texture.get()), std::format("_{}", fontSize) });
        font.m_BitmapTextures.push_back(std::format("{}/{}_{}.sass", outputPath, font.GetName(), fontSize));
    }

    bool FontBlueprint::LoadFont(stbtt_fontinfo& font,
                                 void** fontData,
                                 const std::string& path)
    {
        size_t dataSize;
        *fontData = io::VFS::Get().ReadBinary(path, dataSize);

        if (!stbtt_InitFont(&font, static_cast<const unsigned char*>(*fontData), 0))
        {
            debug::Log::Error("Failed to load font {}", path);
            std::free(*fontData);
            return false;
        }

        return true;
    }

    std::vector<std::pair<ui::FloatRect, int>> FontBlueprint::CollectSortedSDFBoundingBoxes(stbtt_fontinfo& font,
                                                                                         float scale)
    {
        size_t numChars = strlen(s_FontMapChars);

        std::vector<std::pair<ui::FloatRect, int>> ret;
        ret.reserve(numChars);
        for (size_t i = 0; i < numChars; ++i)
        {
            int sizeX = 0, sizeY = 0;
            int xOff = 0, yOff = 0;
            stbtt_GetCodepointSDF(&font,
                                  scale,
                                  s_FontMapChars[i],
                                  SDFPadding,
                                  s_SDFOneEdge,
                                  8,
                                  &sizeX,
                                  &sizeY,
                                  &xOff,
                                  &yOff);
            ret.push_back(std::make_pair(ui::FloatRect { math::Vec2(static_cast<float>(xOff), static_cast<float>(yOff)),
                                                                math::Vec2(static_cast<float>(sizeX), static_cast<float>(sizeY)) }, static_cast<int>(i)));
        }

        std::ranges::sort(ret, [](const std::pair<ui::FloatRect, int>& a,
                                  const std::pair<ui::FloatRect, int>& b)
        {
            return a.first.size > b.first.size;
        });

        return ret;
    }

    std::vector<std::pair<ui::FloatRect, int>> FontBlueprint::CollectSortedBitmapBoundingBoxes(stbtt_fontinfo& font,
                                                                                     float scale)
    {
        size_t numChars = strlen(s_FontMapChars);

        std::vector<std::pair<ui::FloatRect, int>> ret;
        ret.reserve(numChars);
        for (size_t i = 0; i < numChars; ++i)
        {
            int sizeX = 0, sizeY = 0;
            int xOff = 0, yOff = 0;
            stbtt_GetCodepointBitmap(&font,
                                  scale,
                                  scale,
                                  s_FontMapChars[i],
                                  &sizeX,
                                  &sizeY,
                                  &xOff,
                                  &yOff);
            ret.push_back(std::make_pair(ui::FloatRect { math::Vec2(static_cast<float>(xOff), static_cast<float>(yOff)),
                                                                math::Vec2(static_cast<float>(sizeX), static_cast<float>(sizeY)) }, static_cast<int>(i)));
        }

        std::ranges::sort(ret, [](const std::pair<ui::FloatRect, int>& a,
                                  const std::pair<ui::FloatRect, int>& b)
        {
            return a.first.size > b.first.size;
        });

        return ret;
    }

    void FontBlueprint::CollectCharMetrics(stbtt_fontinfo& font,
                                           char c,
                                           float scale,
                                           float ascent,
                                           CharData& charData)
    {
        int advanceWidth = 0, leftSideBearing = 0;
        stbtt_GetCodepointHMetrics(&font, c, &advanceWidth, &leftSideBearing);
        charData.advanceWidth = advanceWidth * scale;
        charData.leftSideBearing = leftSideBearing * scale;
        charData.yOffset = roundf(ascent * scale) + charData.rect.topLeft.y;

        for (size_t j = 0; j < strlen(s_FontMapChars); ++j)
        {
            int kern;
            kern = stbtt_GetCodepointKernAdvance(&font, c, s_FontMapChars[j]);
            if (kern != 0)
            {
                charData.kerning[s_FontMapChars[j]] = kern * scale;
            }
        }
    }

    void FontBlueprint::PackChar(ui::FloatRect FloatRect,
                                 std::vector<ui::FloatRect>& placedRects,
                                 int& imageWidth,
                                 int& imageHeight,
                                 int& scanlineDelta)
    {
        FloatRect.topLeft = { 0, 0 };
        FloatRect.size += { 0, 0 };

        while (true)
        {
            bool canFit = FloatRect.topLeft.x + FloatRect.size.x < imageWidth &&
                          FloatRect.topLeft.y + FloatRect.size.y < imageHeight;

            bool foundPosition = canFit;
            if (canFit)
            {
                for (const auto& otherBB: placedRects)
                {
                    if (otherBB.Overlaps(FloatRect))
                    {
                        foundPosition = false;
                        break;
                    }
                }
            }

            if (foundPosition)
            {
                placedRects.push_back(FloatRect);
                break;
            }
            else
            {
                FloatRect.topLeft.x += scanlineDelta;
                if (FloatRect.topLeft.x >= imageWidth)
                {
                    FloatRect.topLeft.x = 0;
                    FloatRect.topLeft.y += scanlineDelta;

                    if (FloatRect.topLeft.y + FloatRect.size.y > imageHeight)
                    {
                        imageWidth = static_cast<int>(bits::RoundUpToPowerOf2(static_cast<uint32_t>(imageWidth) + 1u));
                        imageHeight = static_cast<int>(
                            bits::RoundUpToPowerOf2(static_cast<uint32_t>(imageHeight) + 1u));
                        scanlineDelta++;
                        FloatRect.topLeft.x = 0;
                        FloatRect.topLeft.y = 0;
                    }
                }
            }
        }
    }

    memory::BinaryBlob FontBlueprint::GenerateBitmap(stbtt_fontinfo& font,
        int width,
        int height,
        float scale,
        const std::vector<std::pair<ui::FloatRect, int>>& boundingBoxes,
        const std::vector<ui::FloatRect>& placedBoundingBoxes)
    {
        uint8_t* bitmap = static_cast<uint8_t*>(std::malloc(width * height));
        memset(bitmap, 0, width * height);
        size_t numChars = strlen(s_FontMapChars);
        for (size_t i = 0; i < numChars; ++i)
        {
            const auto& FloatRect = placedBoundingBoxes[i];

            int sizeX = 0, sizeY = 0;
            int xOff = 0, yOff = 0;
            uint8_t* pixels = stbtt_GetCodepointBitmap(&font,
                                                    scale,
                                                    scale,
                                                    s_FontMapChars[boundingBoxes[i].second],
                                                    &sizeX,
                                                    &sizeY,
                                                    &xOff,
                                                    &yOff);

            for (int j = 0; j < sizeY; ++j)
            {
                int byteOffset = static_cast<int>(FloatRect.topLeft.x + ((FloatRect.topLeft.y + j) * width));
                std::memcpy(bitmap + byteOffset, pixels + j * sizeX, sizeX);
            }
        }

        return memory::BinaryBlob(bitmap, width * height);
    }

    memory::BinaryBlob FontBlueprint::GenerateSDFBitmap(stbtt_fontinfo& font,
                                                        int width,
                                                        int height,
                                                        float scale,
                                                        const std::vector<std::pair<ui::FloatRect, int>>& boundingBoxes,
                                                        const std::vector<ui::FloatRect>& placedBoundingBoxes)
    {
        uint8_t* bitmap = static_cast<uint8_t*>(std::malloc(width * height));
        memset(bitmap, 0, width * height);
        size_t numChars = strlen(s_FontMapChars);
        for (size_t i = 0; i < numChars; ++i)
        {
            const auto& FloatRect = placedBoundingBoxes[i];

            int sizeX = 0, sizeY = 0;
            int xOff = 0, yOff = 0;
            uint8_t* pixels = stbtt_GetCodepointSDF(&font,
                                                    scale,
                                                    s_FontMapChars[boundingBoxes[i].second],
                                                    SDFPadding,
                                                    s_SDFOneEdge,
                                                    8,
                                                    &sizeX,
                                                    &sizeY,
                                                    &xOff,
                                                    &yOff);

            for (int j = 0; j < sizeY; ++j)
            {
                int byteOffset = static_cast<int>(FloatRect.topLeft.x + ((FloatRect.topLeft.y + j) * width));
                std::memcpy(bitmap + byteOffset, pixels + j * sizeX, sizeX);
            }
        }

        return memory::BinaryBlob(bitmap, width * height);
    }
}
