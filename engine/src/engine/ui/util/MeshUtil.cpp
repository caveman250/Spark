#include "MeshUtil.h"

#include "engine/asset/builder/FontBlueprint.h"
#include "engine/asset/font/Font.h"

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

    math::Vec2 ApplyWrapping(math::Vec2 cursorPos,
                       char c,
                       text::WrapMode mode,
                       size_t charIndex,
                       const Rect& rect,
                       const std::shared_ptr<asset::Font>& font,
                       int fontSize,
                       const std::string& text,
                       float scale,
                       bool& didWrap)
    {
        if (c == '\n')
        {
            cursorPos.x = 0;
            cursorPos.y += font->GetLineHeight(fontSize);
            didWrap = true;
        }

        if (c == ' ' || mode == text::WrapMode::Char)
        {
            if (!didWrap)
            {
                size_t lookAhead = charIndex + 1;
                float xCopy = cursorPos.x;

                char nextChar = text[lookAhead];
                while (nextChar != ' ' && lookAhead < text.size() - 1)
                {
                    if (nextChar == '\n')
                    {
                        break;
                    }

                    const auto& nextCharData = font->GetCharData(nextChar);

                    if (xCopy + nextCharData.advanceWidth * scale >= rect.size.x - 1)
                    {
                        cursorPos.x = 0;
                        cursorPos.y += font->GetLineHeight(fontSize);
                        didWrap = true;
                        break;
                    }

                    if (mode == text::WrapMode::Char)
                    {
                        break;
                    }

                    xCopy += nextCharData.advanceWidth * scale;
                    nextChar = text[++lookAhead];
                }
            }
        }

        return cursorPos;
    }

    float CalculateJustificationXOffset(text::Alignment justification,
                            float endOfLineX,
                            const Rect& rect)
    {
        switch (justification)
        {
            case text::Alignment::Left:
                return 0;
            case text::Alignment::Center:
            {
                float availableSpace = rect.size.x - endOfLineX;
                return availableSpace / 2.f;
            }
            case text::Alignment::Right:
            {
                return rect.size.x - endOfLineX;
            }
            default:
                SPARK_ASSERT(false, "CalculateJustificationXOffset - Unhandled Justification.");
                return 0;
        }
    }

    math::Vec2 ApplyKerning(math::Vec2 cursorPos,
                               size_t index,
                               const std::string& text,
                               const asset::CharData& charData,
                               float scale)
    {
        if (index < text.size() - 1)
        {
            char nextChar = text[index + 1];
            if (charData.kerning.contains(nextChar))
            {
                cursorPos.x += charData.kerning.at(nextChar) * scale;
            }
        }

        return cursorPos;
    }

    math::Vec2 ApplyLeftSideBearing(math::Vec2 cursorPos,
                               const asset::CharData& charData,
                               float scale)
    {
        cursorPos.x += charData.leftSideBearing * scale;
        return cursorPos;
    }

    math::Vec2 ApplyAdvanceWidth(math::Vec2 cursorPos,
                                       const asset::CharData& charData,
                                       float scale)
    {
        cursorPos.x += charData.advanceWidth * scale;
        return cursorPos;
    }

    void CreateCharMesh(const asset::CharData& charData,
                        const math::Vec2& cursorPos,
                        asset::StaticMesh& mesh,
                        uint32_t& indexOffset,
                        float scale)
    {
        math::Vec2 TL = charData.rect.topLeft * scale + cursorPos;
        math::Vec2 BR = TL + charData.rect.size * scale;

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
    }

    asset::StaticMesh CreateTextMesh(const Rect& rect,
                                     const std::shared_ptr<asset::Font>& font,
                                     int fontSize,
                                     const std::string& text,
                                     bool applyKerning,
                                     text::WrapMode wrap,
                                     text::Alignment justification)
    {
        float scale = static_cast<float>(fontSize) / asset::builder::FontBlueprint::s_Scale;
        asset::StaticMesh mesh;
        uint32_t indexOffset = 0;
        math::Vec2 cursorPos = { };
        cursorPos.y += font->GetAscent(fontSize);
        size_t lineStart = 0;
        for (size_t i = 0; i < text.size(); ++i)
        {
            char c = text[i];
            if (c != '\n')
            {
                const auto& charData = font->GetCharData(c);

                if (applyKerning)
                {
                    cursorPos = ApplyKerning(cursorPos, i, text, charData, scale);
                }
                cursorPos = ApplyLeftSideBearing(cursorPos, charData, scale);
                CreateCharMesh(charData, cursorPos, mesh, indexOffset, scale);
                cursorPos = ApplyAdvanceWidth(cursorPos, charData, scale);
            }

            if (wrap == text::WrapMode::Word ||
                wrap == text::WrapMode::Char ||
                wrap == text::WrapMode::WordChar)
            {
                bool didWrap = false;
                float oldX = cursorPos.x;
                cursorPos = ApplyWrapping(cursorPos,
                                          c,
                                          wrap,
                                          i,
                                          rect,
                                          font,
                                          fontSize,
                                          text,
                                          scale,
                                          didWrap);

                if (!didWrap && wrap == text::WrapMode::WordChar && cursorPos.x > rect.size.x)
                {
                    cursorPos = ApplyWrapping(cursorPos,
                                          c,
                                          text::WrapMode::Char,
                                          i,
                                          rect,
                                          font,
                                          fontSize,
                                          text,
                                          scale,
                                          didWrap);
                }

                if (didWrap)
                {
                    float offset = CalculateJustificationXOffset(justification,
                                       oldX,
                                       rect);
                    if (offset != 0)
                    {
                        size_t lineEnd = i;
                        for (size_t j = lineStart * 4; j < lineEnd * 4; ++j)
                        {
                            mesh.vertices[j].x += offset;
                        }
                    }
                    lineStart = i + 1;
                }


                if (!didWrap && i == text.size() - 1)
                {
                    float offset = CalculateJustificationXOffset(justification,
                                                               cursorPos.x,
                                                               rect);
                    if (offset != 0)
                    {
                        size_t lineEnd = i + 1;
                        for (size_t j = lineStart * 4; j < lineEnd * 4; ++j)
                        {
                            mesh.vertices[j].x += offset;
                        }
                    }
                }
            }
            else if (i == text.size() - 1)
            {
                float offset = CalculateJustificationXOffset(justification,
                                                           cursorPos.x,
                                                           rect);
                if (offset != 0)
                {
                    size_t lineEnd = i + 1;
                    for (size_t j = lineStart * 4; j < lineEnd * 4; ++j)
                    {
                        mesh.vertices[j].x += offset;
                    }
                }
            }
        }


        return mesh;
    }

    math::Vec2 MeasureText(const Rect& bounds,
                              const std::shared_ptr<asset::Font>& font,
                              int fontSize,
                              const std::string& text,
                              bool applyKerning,
                              text::WrapMode wrap)
    {
        return MeasureText(bounds, font, fontSize, text, applyKerning, wrap, text.size());
    }

    math::Vec2 MeasureText(const Rect& bounds,
        const std::shared_ptr<asset::Font>& font,
        int fontSize,
        const std::string& text,
        bool applyKerning,
        text::WrapMode wrap,
        size_t endIndex)
    {
        float scale = static_cast<float>(fontSize) / asset::builder::FontBlueprint::s_Scale;
        math::Vec2 max = { };

        math::Vec2 cursorPos = { };
        cursorPos.y += font->GetLineHeight(fontSize);
        for (size_t i = 0; i < endIndex; ++i)
        {
            char c = text[i];
            if (c != '\n')
            {
                const auto& charData = font->GetCharData(c);

                if (applyKerning)
                {
                    cursorPos = ApplyKerning(cursorPos, i, text, charData, scale);
                }
                cursorPos = ApplyLeftSideBearing(cursorPos, charData, scale);

                math::Vec2 TL = charData.rect.topLeft * scale + cursorPos;
                math::Vec2 BR = TL + charData.rect.size * scale;
                max = math::Vec2(std::max(BR.x, max.x), std::max(BR.y, max.y));

                cursorPos = ApplyAdvanceWidth(cursorPos, charData, scale);
            }

            if (wrap != text::WrapMode::None)
            {
                bool didWrap = false;
                cursorPos = ApplyWrapping(cursorPos,
                                          c,
                                          wrap,
                                          i,
                                          bounds,
                                          font,
                                          fontSize,
                                          text,
                                          scale,
                                          didWrap);

                if (!didWrap && wrap == text::WrapMode::WordChar && cursorPos.x > bounds.size.x)
                {
                    cursorPos = ApplyWrapping(cursorPos,
                                          c,
                                          text::WrapMode::Char,
                                          i,
                                          bounds,
                                          font,
                                          fontSize,
                                          text,
                                          scale,
                                          didWrap);
                }
                //Alignment does not affect desired size.
            }
        }
        return max;
    }

    int GetCharIndexForPosition(const math::Vec2& pos,
        const Rect& bounds,
        const std::shared_ptr<asset::Font>& font,
        int fontSize,
        const std::string& text,
        bool applyKerning,
        text::WrapMode wrap,
        text::Alignment justification)
    {
        float scale = static_cast<float>(fontSize) / asset::builder::FontBlueprint::s_Scale;
        math::Vec2 cursorPos = { };
        cursorPos.y += font->GetLineHeight(fontSize);
        size_t lineStart = 0;
        std::vector<Rect> boundingBoxes = {};
        for (size_t i = 0; i < text.size(); ++i)
        {
            char c = text[i];
            const auto& charData = font->GetCharData(c);

            if (applyKerning)
            {
                cursorPos = ApplyKerning(cursorPos, i, text, charData, scale);
            }
            cursorPos = ApplyLeftSideBearing(cursorPos, charData, scale);

            math::Vec2 TL = charData.rect.topLeft * scale + cursorPos;
            math::Vec2 BR = TL + charData.rect.size * scale;
            boundingBoxes.push_back(Rect(TL, BR));

            cursorPos = ApplyAdvanceWidth(cursorPos, charData, scale);

            if (wrap != text::WrapMode::None)
            {
                bool didWrap = false;
                float oldX = cursorPos.x;
                cursorPos = ApplyWrapping(cursorPos,
                                          c,
                                          wrap,
                                          i,
                                          bounds,
                                          font,
                                          fontSize,
                                          text,
                                          scale,
                                          didWrap);

                if (!didWrap && wrap == text::WrapMode::WordChar && cursorPos.x > bounds.size.x)
                {
                    cursorPos = ApplyWrapping(cursorPos,
                                          c,
                                          text::WrapMode::Char,
                                          i,
                                          bounds,
                                          font,
                                          fontSize,
                                          text,
                                          scale,
                                          didWrap);
                }

                if (didWrap)
                {
                    float offset = CalculateJustificationXOffset(justification,
                                                               oldX,
                                                               bounds);
                    size_t lineEnd = i;
                    float halfFontSize = fontSize * .5f;
                    for (size_t j = lineStart; j < lineEnd; ++j)
                    {
                        const auto& charBounds = boundingBoxes[j];
                        if (std::abs(charBounds.topLeft.x + offset - pos.x) < halfFontSize &&
                            std::abs(charBounds.topLeft.y - pos.y) <= fontSize)
                        {
                            return static_cast<int>(j);
                        }
                    }

                    lineStart = i + 1;
                }

            }
        }

        float offset = CalculateJustificationXOffset(justification,
                                                   cursorPos.x,
                                                   bounds);
        float halfFontSize = fontSize * .5f;
        for (size_t j = lineStart; j < text.size(); ++j)
        {
            const auto& charBounds = boundingBoxes[j];
            if (std::abs(charBounds.topLeft.x + offset - pos.x) < halfFontSize &&
                std::abs(charBounds.topLeft.y - pos.y) <= fontSize)
            {
                return static_cast<int>(j);
            }
        }

        return static_cast<int>(text.size());
    }
}
