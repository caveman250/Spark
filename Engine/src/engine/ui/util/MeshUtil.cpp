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

    math::IntVec2 ApplyWrapping(math::IntVec2 cursorPos,
                       char c,
                       text::WrapMode::Type mode,
                       size_t charIndex,
                       const Rect& rect,
                       const std::shared_ptr<asset::Font>& font,
                       int fontSize,
                       const String& text,
                       bool& didWrap)
    {
        if (c == ' ' || mode == text::WrapMode::Char)
        {
            size_t lookAhead = charIndex + 1;
            int xCopy = cursorPos.x;

            char nextChar = text[lookAhead];
            while (nextChar != ' ' && lookAhead < text.Size() - 1)
            {
                const auto& nextCharData = font->GetCharData(fontSize, nextChar);

                if (xCopy + nextCharData.advanceWidth >= rect.size.x)
                {
                    cursorPos.x = 0;
                    cursorPos.y += fontSize;
                    didWrap = true;
                    break;
                }

                if (mode == text::WrapMode::Char)
                {
                    break;
                }

                xCopy += nextCharData.advanceWidth;
                nextChar = text[++lookAhead];
            }
        }

        return cursorPos;
    }

    int CalculateJustificationXOffset(ui::text::Alignment::Type justification,
                            int endOfLineX,
                            const Rect& rect)
    {
        switch (justification)
        {
            case text::Alignment::Left:
                return 0;
            case text::Alignment::Center:
            {
                int availableSpace = rect.size.x - endOfLineX;
                return availableSpace / 2;
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

    math::IntVec2 ApplyKerning(math::IntVec2 cursorPos,
                               size_t index,
                               const String& text,
                               const asset::CharData& charData)
    {
        if (index < text.Size() - 1)
        {
            char nextChar = text[index + 1];
            if (charData.kerning.contains(nextChar))
            {
                cursorPos.x += charData.kerning.at(nextChar);
            }
        }

        return cursorPos;
    }

    math::IntVec2 ApplyLeftSideBearing(math::IntVec2 cursorPos,
                               const asset::CharData& charData)
    {
        cursorPos.x += charData.leftSideBearing;
        return cursorPos;
    }

    math::IntVec2 ApplyAdvanceWidth(math::IntVec2 cursorPos,
                                       const asset::CharData& charData)
    {
        cursorPos.x += charData.advanceWidth;
        return cursorPos;
    }

    void CreateCharMesh(const asset::CharData& charData,
                        const math::IntVec2& cursorPos,
                        asset::StaticMesh& mesh,
                        uint32_t& indexOffset)
    {
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
    }

    asset::StaticMesh CreateTextMesh(const Rect& rect,
                                     const std::shared_ptr<asset::Font>& font,
                                     int fontSize,
                                     const String& text,
                                     bool applyKerning,
                                     text::WrapMode::Type wrap,
                                     text::Alignment::Type justification)
    {
        asset::StaticMesh mesh;
        uint32_t indexOffset = 0;
        math::IntVec2 cursorPos = { };
        cursorPos.y += fontSize;
        size_t lineStart = 0;
        for (size_t i = 0; i < text.Size(); ++i)
        {
            char c = text[i];
            const auto& charData = font->GetCharData(fontSize, c);

            if (applyKerning)
            {
                cursorPos = ApplyKerning(cursorPos, i, text, charData);
            }
            cursorPos = ApplyLeftSideBearing(cursorPos, charData);
            CreateCharMesh(charData, cursorPos, mesh, indexOffset);
            cursorPos = ApplyAdvanceWidth(cursorPos, charData);

            if (wrap == text::WrapMode::Word ||
                wrap == text::WrapMode::Char ||
                wrap == text::WrapMode::WordChar)
            {
                bool didWrap = false;
                int oldX = cursorPos.x;
                cursorPos = ApplyWrapping(cursorPos,
                                          c,
                                          wrap,
                                          i,
                                          rect,
                                          font,
                                          fontSize,
                                          text,
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
                                          didWrap);
                }

                if (didWrap)
                {
                    int offset = CalculateJustificationXOffset(justification,
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


                if (!didWrap && i == text.Size() - 1)
                {
                    int offset = CalculateJustificationXOffset(justification,
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
            else if (i == text.Size() - 1)
            {
                int offset = CalculateJustificationXOffset(justification,
                                                           cursorPos.x,
                                                           rect);
                if (offset != 0)
                {
                    size_t lineEnd = i;
                    for (size_t j = lineStart * 4; j <= lineEnd * 4; ++j)
                    {
                        mesh.vertices[j].x += offset;
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
                              text::WrapMode::Type wrap)
    {
        return MeasureText(bounds, font, fontSize, text, applyKerning, wrap, text.Size());
    }

    math::IntVec2 MeasureText(const Rect& bounds,
        const std::shared_ptr<asset::Font>& font,
        int fontSize,
        const String& text,
        bool applyKerning,
        text::WrapMode::Type wrap,
        size_t endIndex)
    {
        math::IntVec2 max = { };

        math::IntVec2 cursorPos = { };
        cursorPos.y += fontSize;
        for (size_t i = 0; i < endIndex; ++i)
        {
            char c = text[i];
            const auto& charData = font->GetCharData(fontSize, c);

            if (applyKerning)
            {
                cursorPos = ApplyKerning(cursorPos, i, text, charData);
            }
            cursorPos = ApplyLeftSideBearing(cursorPos, charData);

            math::IntVec2 TL = charData.rect.topLeft + cursorPos;
            math::IntVec2 BR = TL + charData.rect.size;
            max = math::IntVec2(std::max(BR.x, max.x), std::max(BR.y, max.y));

            cursorPos = ApplyAdvanceWidth(cursorPos, charData);

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
                                          didWrap);
                }
                //Alignment does not effect desired size.
            }
        }
        return max;
    }

    int GetCharIndexForPosition(const math::Vec2& pos,
        const Rect& bounds,
        const std::shared_ptr<asset::Font>& font,
        int fontSize,
        const String& text,
        bool applyKerning,
        text::WrapMode::Type wrap,
        text::Alignment::Type justification)
    {
        math::IntVec2 cursorPos = { };
        cursorPos.y += fontSize;
        size_t lineStart = 0;
        std::vector<Rect> boundingBoxes = {};
        for (size_t i = 0; i < text.Size(); ++i)
        {
            char c = text[i];
            const auto& charData = font->GetCharData(fontSize, c);

            if (applyKerning)
            {
                cursorPos = ApplyKerning(cursorPos, i, text, charData);
            }
            cursorPos = ApplyLeftSideBearing(cursorPos, charData);

            math::IntVec2 TL = charData.rect.topLeft + cursorPos;
            math::IntVec2 BR = TL + charData.rect.size;
            boundingBoxes.push_back(Rect(TL, BR));

            cursorPos = ApplyAdvanceWidth(cursorPos, charData);

            if (wrap != text::WrapMode::None)
            {
                bool didWrap = false;
                int oldX = cursorPos.x;
                cursorPos = ApplyWrapping(cursorPos,
                                          c,
                                          wrap,
                                          i,
                                          bounds,
                                          font,
                                          fontSize,
                                          text,
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
                                          didWrap);
                }

                if (didWrap)
                {
                    int offset = CalculateJustificationXOffset(justification,
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

        int offset = CalculateJustificationXOffset(justification,
                                                   cursorPos.x,
                                                   bounds);
        float halfFontSize = fontSize * .5f;
        for (size_t j = lineStart; j < text.Size(); ++j)
        {
            const auto& charBounds = boundingBoxes[j];
            if (std::abs(charBounds.topLeft.x + offset - pos.x) < halfFontSize &&
                std::abs(charBounds.topLeft.y - pos.y) <= fontSize)
            {
                return static_cast<int>(j);
            }
        }

        return static_cast<int>(text.Size());
    }
}
