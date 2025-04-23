#include "MeshUtil.h"

#include "engine/asset/font/Font.h"
#include "engine/string/String.h"

namespace se::ui::util
{
    asset::StaticMesh CreateMeshFromRect(const Rect& rect)
    {
        asset::StaticMesh mesh;
        mesh.vertices =
        {
            { 0.f, static_cast<float>(rect.size.y), 0 },
            { static_cast<float>(rect.size.x), static_cast<float>(rect.size.y), 0 },
            { static_cast<float>(rect.size.x), 0.f, 0 },
            { 0.f, 0.f, 0 },
        };
        mesh.indices = { 1, 3, 0, 3, 1, 2 };
        mesh.uvs =
        {
            { 0, 1 },
            { 1, 1 },
            { 1, 0 },
            { 0, 0 }
        };
        return mesh;
    }

    asset::StaticMesh CreateTextMesh(const Rect& rect,
                                     const std::shared_ptr<asset::Font>& font,
                                     int fontSize,
                                     const String& text,
                                     bool applyKerning,
                                     bool wrap)
    {
        asset::StaticMesh mesh;
        uint32_t indexOffset = 0;
        math::IntVec2 cursorPos = { };
        cursorPos.y += fontSize;
        for (size_t i = 0; i < text.Size(); ++i)
        {
            char c = text[i];
            const auto& charData = font->GetCharData(fontSize, c);

            if (applyKerning && i < text.Size() - 1)
            {
                char nextChar = text[i + 1];
                if (charData.kerning.contains(nextChar))
                {
                    cursorPos.x += charData.kerning.at(nextChar);
                }
            }

            cursorPos.x += charData.leftSideBearing;

            math::IntVec2 TL = charData.rect.topLeft + cursorPos;
            math::IntVec2 BR = TL + charData.rect.size;
            mesh.vertices.push_back({ (float) TL.x, (float) BR.y, 0 });
            mesh.vertices.push_back({ (float) BR.x, (float) BR.y, 0 });
            mesh.vertices.push_back({ (float) BR.x, (float) TL.y, 0 });
            mesh.vertices.push_back({ (float) TL.x, (float) TL.y, 0 });

            mesh.indices.insert(mesh.indices.end(), { indexOffset + 1, indexOffset + 3, indexOffset, indexOffset + 3, indexOffset + 1, indexOffset + 2 });
            indexOffset += 4;

            mesh.uvs.push_back({ charData.uvTopLeft.x, charData.uvBottomRight.y });
            mesh.uvs.push_back({ charData.uvBottomRight.x, charData.uvBottomRight.y });
            mesh.uvs.push_back({ charData.uvBottomRight.x, charData.uvTopLeft.y });
            mesh.uvs.push_back({ charData.uvTopLeft.x, charData.uvTopLeft.y });

            cursorPos.x += charData.advanceWidth;

            if (wrap)
            {
                if (c == ' ')
                {
                    size_t lookAhead = i + 1;
                    float xCopy = cursorPos.x;

                    char nextChar = text[lookAhead];
                    while (nextChar != ' ' && lookAhead < text.Size() - 1)
                    {
                        const auto& nextCharData = font->GetCharData(fontSize, nextChar);

                        if (xCopy + nextCharData.advanceWidth >= rect.size.x)
                        {
                            cursorPos.x = 0.f;
                            cursorPos.y += fontSize;
                            break;
                        }
                        xCopy += nextCharData.advanceWidth;
                        nextChar = text[++lookAhead];
                    }
                }
            }
        }
        return mesh;
    }

    math::IntVec2 MeasureText(const Rect& bounds,
                              const std::shared_ptr<asset::Font>& font,
                              int fontSize,
                              const String& text,
                              bool applyKerning,
                              bool wrap)
    {
        return MeasureText(bounds, font, fontSize, text, applyKerning, wrap, text.Size());
    }

    math::IntVec2 MeasureText(const Rect& bounds,
        const std::shared_ptr<asset::Font>& font,
        int fontSize,
        const String& text,
        bool applyKerning,
        bool wrap,
        int endIndex)
    {
        math::IntVec2 max = { };

        math::IntVec2 cursorPos = { };
        cursorPos.y += fontSize;
        for (size_t i = 0; i < endIndex; ++i)
        {
            char c = text[i];
            const auto& charData = font->GetCharData(fontSize, c);

            if (applyKerning && i < text.Size() - 1)
            {
                char nextChar = text[i + 1];
                if (charData.kerning.contains(nextChar))
                {
                    cursorPos.x += charData.kerning.at(nextChar);
                }
            }

            cursorPos.x += charData.leftSideBearing;

            math::IntVec2 TL = charData.rect.topLeft + cursorPos;
            math::IntVec2 BR = TL + charData.rect.size;
            max = math::IntVec2(std::max(BR.x, max.x), std::max(BR.y, max.y));

            cursorPos.x += charData.advanceWidth;

            if (wrap)
            {
                if (c == ' ')
                {
                    size_t lookAhead = i + 1;
                    float xCopy = cursorPos.x;

                    char nextChar = text[lookAhead];
                    while (nextChar != ' ' && lookAhead < text.Size() - 1)
                    {
                        const auto& nextCharData = font->GetCharData(fontSize, nextChar);

                        if (xCopy + nextCharData.advanceWidth >= bounds.size.x)
                        {
                            cursorPos.x = 0.f;
                            cursorPos.y += fontSize;
                            break;
                        }
                        xCopy += nextCharData.advanceWidth;
                        nextChar = text[++lookAhead];
                    }
                }
            }
        }
        return max;
    }
}
