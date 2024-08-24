#include "spark.h"
#include "FontBlueprint.h"
#include "engine/bits/PowerOfTwo.h"
#include "engine/io/VFS.h"
#include "engine/reflect/Util.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "stb_image.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "TextureBlueprint.h"
#include "engine/asset/font/Font.h"
#include "engine/asset/texture/Texture.h"
#include "engine/ui/Rect.h"
#include "engine/asset/builder/util/Bitmap.h"

namespace se::asset::builder
{
    constexpr const char *s_FontMapChars =
            "!@#$%^&*()_+"
            "1234567890-="
            "qwertyuiop[]"
            "QWERTYUIOP{}"
            "asdfghjkl;'\\"
            "ASDFGHJKL:\"|"
            "zxcvbnm,./`"
            "ZXCVBNM<>?~/ ";

    static std::vector s_FontSizes =
    {
        10,
        11,
        12,
        14,
        16,
        18,
        21,
        24,
        30,
        36,
        48,
        60,
        72,
        96
    };

    std::regex FontBlueprint::GetFilePattern() const
    {
        return std::regex(".*.ttf");
    }

    std::vector<BuiltAsset> FontBlueprint::BuildAsset(const std::string &path, const std::string &outputPath,
                                                      meta::MetaData &) const
    {
        std::vector<BuiltAsset> ret;
        Font font;

        stbtt_fontinfo info = {};
        void *fontData = nullptr;
        if (!LoadFont(info, &fontData, path))
        {
            return {};
        }

        auto lastSlashIt = outputPath.find_last_of('/');
        auto extensionIt = outputPath.find_last_of('.');
        std::string outputDir = outputPath.substr(0, lastSlashIt);
        std::string fontName = outputPath.substr(lastSlashIt + 1, extensionIt - 1 - lastSlashIt);

        font.m_Name = fontName;
        std::mutex fontAssetLock;
        auto addFontSize = [&fontAssetLock, &font, outputDir, fontName](
            int fontSize, const std::unordered_map<char, CharData> &charData)
        {
            auto lock = std::lock_guard(fontAssetLock);

            auto &fontAssetData = font.m_AssetData[fontSize];
            fontAssetData.path = std::format("{}/{}_{}.sass", outputDir, fontName, fontSize);
            fontAssetData.charData = charData;
        };

        std::mutex retLock;
        auto addReturnItem = [&retLock, &ret](const std::shared_ptr<binary::Database> &db,
                                              const std::string fileNameSuffix)
        {
            auto lock = std::lock_guard(retLock);
            ret.push_back({db, fileNameSuffix});
        };

        int ascent = GetAscent(info);
        std::for_each(std::execution::par_unseq, s_FontSizes.begin(), s_FontSizes.end(),
        [&info, addReturnItem, addFontSize, ascent](const auto &fontSize)
        {
            float scale = stbtt_ScaleForPixelHeight(&info, static_cast<float>(fontSize));

            size_t numChars = strlen(s_FontMapChars);
            auto boundingBoxes = CollectSortedBoundingBoxes(info, scale);
            std::vector<ui::Rect> placedBoundingBoxes = {};
            placedBoundingBoxes.reserve(numChars);

            std::unordered_map<char, CharData> charDataMap;
            int imageWidth = 128;
            int imageHeight = 128;
            int interval = 1;
            for (size_t i = 0; i < numChars; ++i)
            {
                char c = s_FontMapChars[boundingBoxes[i].second];
                auto &charData = charDataMap[c];
                charData.rect = boundingBoxes[i].first;

                CollectCharMetrics(info, c, scale, static_cast<float>(ascent), charData);
                PackChar(charData.rect, placedBoundingBoxes, charData, imageWidth, imageHeight, interval);
            }

            for (size_t i = 0; i < numChars; ++i)
            {
                char c = s_FontMapChars[boundingBoxes[i].second];
                const auto& rect = placedBoundingBoxes[i];
                auto &charData = charDataMap[c];
                // calculate Uvs now that the image size is stable
                charData.uvTopLeft = math::Vec2(static_cast<float>(rect.topLeft.x) / imageWidth,
                                              static_cast<float>(rect.topLeft.y) / imageHeight);
                charData.uvBottomRight = math::Vec2((static_cast<float>(rect.topLeft.x) + static_cast<float>(rect.size.x)) / imageWidth,
                                                    (static_cast<float>(rect.topLeft.y) + static_cast<float>(rect.size.y)) / imageHeight);
            }

            addFontSize(fontSize, charDataMap);
            memory::BinaryBlob monochromeBitmap = GenerateMonochromeBitmap(info, imageWidth, imageHeight, scale,
                                                               boundingBoxes, placedBoundingBoxes);

            auto texture = Texture::FromRawData(imageWidth, imageHeight, monochromeBitmap, texture::Format::R8);
            addReturnItem(reflect::SerialiseType<Texture>(texture.get()), std::format("_{}", fontSize));
        });

        std::free(fontData);

        ret.push_back({reflect::SerialiseType<Font>(&font), ""});

        return ret;
    }

    bool FontBlueprint::LoadFont(stbtt_fontinfo &font, void **fontData, const std::string &path)
    {
        size_t dataSize;
        *fontData = io::VFS::Get().ReadBinary(path, dataSize);

        if (!stbtt_InitFont(&font, static_cast<const unsigned char *>(*fontData), 0))
        {
            debug::Log::Error("Failed to load font {}", path);
            std::free(fontData);
            return false;
        }

        return true;
    }

    int FontBlueprint::GetAscent(stbtt_fontinfo &font)
    {
        int ascent, descent, lineGap;
        stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
        return ascent;
    }

    std::vector<std::pair<ui::Rect, int> > FontBlueprint::CollectSortedBoundingBoxes(stbtt_fontinfo &font, float scale)
    {
        size_t numChars = strlen(s_FontMapChars);

        std::vector<std::pair<ui::Rect, int> > ret;
        ret.reserve(numChars);
        for (int i = 0; i < numChars; ++i)
        {
            int x1, y1, x2, y2;
            stbtt_GetCodepointBitmapBox(&font, s_FontMapChars[i], scale, scale, &x1, &y1, &x2, &y2);
            ret.push_back(std::make_pair(ui::Rect{math::IntVec2(x1, y1), math::IntVec2(x2 - x1, y2 - y1)}, i));
        }

        std::ranges::sort(ret, [](const std::pair<ui::Rect, int> &a, const std::pair<ui::Rect, int> &b)
        {
            return a.first.size > b.first.size;
        });

        return ret;
    }

    void FontBlueprint::CollectCharMetrics(stbtt_fontinfo &font, char c, float scale, float ascent, CharData &charData)
    {
        stbtt_GetCodepointHMetrics(&font, c, &charData.advanceWidth, &charData.leftSideBearing);
        charData.advanceWidth = static_cast<int>(charData.advanceWidth * scale);
        charData.leftSideBearing = static_cast<int>(charData.leftSideBearing * scale);

        charData.yOffset = static_cast<int>(roundf(ascent * scale) + charData.rect.topLeft.y);

        for (int j = 0; j < strlen(s_FontMapChars); ++j)
        {
            int kern;
            kern = stbtt_GetCodepointKernAdvance(&font, c, s_FontMapChars[j]);
            if (kern != 0)
            {
                charData.kerning[s_FontMapChars[j]] = kern * scale;
            }
        }
    }

    void FontBlueprint::PackChar(ui::Rect rect, std::vector<ui::Rect> &placedRects, CharData &charData, int &imageWidth,
                                 int &imageHeight, int &scanlineDelta)
    {
        rect.topLeft = {0, 0};

        while (true)
        {
            bool canFit = rect.topLeft.x + rect.size.x < imageWidth &&
                          rect.topLeft.y + rect.size.y < imageHeight;

            bool foundPosition = canFit;
            if (canFit)
            {
                for (const auto &otherBB: placedRects)
                {
                    if (otherBB.Overlaps(rect))
                    {
                        foundPosition = false;
                        break;
                    }
                }
            }

            if (foundPosition)
            {
                placedRects.push_back(rect);
                break;
            } else
            {
                rect.topLeft.x += scanlineDelta;
                if (rect.topLeft.x >= imageWidth)
                {
                    rect.topLeft.x = 0;
                    rect.topLeft.y += scanlineDelta;

                    if (rect.topLeft.y + rect.size.y > imageHeight)
                    {
                        imageWidth = static_cast<int>(bits::RoundUpToPowerOf2(static_cast<uint32_t>(imageWidth) + 1u));
                        imageHeight = static_cast<int>(
                            bits::RoundUpToPowerOf2(static_cast<uint32_t>(imageHeight) + 1u));
                        scanlineDelta++;
                        rect.topLeft.x = 0;
                        rect.topLeft.y = 0;
                    }
                }
            }
        }
    }

    memory::BinaryBlob FontBlueprint::GenerateMonochromeBitmap(stbtt_fontinfo &font, int width, int height, float scale,
                                                     const std::vector<std::pair<ui::Rect, int> > &boundingBoxes,
                                                     const std::vector<ui::Rect> &placedBoundingBoxes)
    {
        uint8_t *bitmap = static_cast<uint8_t *>(std::malloc(width * height));
        memset(bitmap, 0, width * height);
        for (int i = 0; i < strlen(s_FontMapChars); ++i)
        {
            const auto &rect = placedBoundingBoxes[i];
            int byteOffset = rect.topLeft.x + (rect.topLeft.y * width);
            stbtt_MakeCodepointBitmap(&font, bitmap + byteOffset, rect.size.x,
                                      rect.size.y, width, scale, scale,
                                      s_FontMapChars[boundingBoxes[i].second]);
        }

        return memory::BinaryBlob(bitmap, width * height);
    }
}
