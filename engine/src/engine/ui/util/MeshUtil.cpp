#include "MeshUtil.h"

#include "EditableTextUtil.h"
#include "engine/asset/builder/FontBlueprint.h"
#include "engine/asset/font/Font.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/EditableTextComponent.h"

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
                       const char c,
                       const text::WrapMode mode,
                       const size_t charIndex,
                       const Rect* rect,
                       const asset::Font* font,
                       const int fontSize,
                       const std::string* text,
                       const float scale,
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
            size_t lookAhead = charIndex + 1;
            if (!didWrap && lookAhead < text->size() - 1)
            {
                float xCopy = cursorPos.x;
                char nextChar = text->at(lookAhead);
                while (nextChar != ' ' && lookAhead < text->size() - 1)
                {
                    if (nextChar == '\n')
                    {
                        break;
                    }

                    const auto& nextCharData = font->GetCharData(nextChar);

                    if (xCopy + nextCharData.advanceWidth * scale >= rect->size.x - 1)
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
                    nextChar = text->at(++lookAhead);
                }
            }
        }

        return cursorPos;
    }

    float CalculateJustificationXOffset(const text::Alignment justification,
                            const float endOfLineX,
                            const Rect* rect)
    {
        switch (justification)
        {
            case text::Alignment::Left:
                return 0;
            case text::Alignment::Center:
            {
                const float availableSpace = rect->size.x - endOfLineX;
                return availableSpace / 2.f;
            }
            case text::Alignment::Right:
            {
                return rect->size.x - endOfLineX;
            }
            default:
                SPARK_ASSERT(false, "CalculateJustificationXOffset - Unhandled Justification.");
                return 0;
        }
    }

    math::Vec2 ApplyKerning(math::Vec2 cursorPos,
                               const size_t index,
                               const std::string* text,
                               const asset::CharData& charData,
                               const float scale)
    {
        if (index < text->size() - 1)
        {
            const char nextChar = text->at(index + 1);
            if (charData.kerning.contains(nextChar))
            {
                cursorPos.x += charData.kerning.at(nextChar) * scale;
            }
        }

        return cursorPos;
    }

    math::Vec2 ApplyLeftSideBearing(math::Vec2 cursorPos,
                               const asset::CharData& charData,
                               const float scale)
    {
        cursorPos.x += charData.leftSideBearing * scale;
        return cursorPos;
    }

    math::Vec2 ApplyAdvanceWidth(math::Vec2 cursorPos,
                                       const asset::CharData& charData,
                                       const float scale)
    {
        cursorPos.x += charData.advanceWidth * scale;
        return cursorPos;
    }

    void CreateCharMesh(const asset::CharData& charData,
                        const math::Vec2& cursorPos,
                        asset::StaticMesh& mesh,
                        uint32_t& indexOffset,
                        const float scale)
    {
        const math::Vec2 TL = charData.rect.topLeft * scale + cursorPos;
        const math::Vec2 BR = TL + charData.rect.size * scale;

        mesh.vertices.push_back({ static_cast<float>(TL.x), static_cast<float>(BR.y), 0 });
        mesh.vertices.push_back({ static_cast<float>(BR.x), static_cast<float>(BR.y), 0 });
        mesh.vertices.push_back({ static_cast<float>(BR.x), static_cast<float>(TL.y), 0 });
        mesh.vertices.push_back({ static_cast<float>(TL.x), static_cast<float>(TL.y), 0 });

        mesh.indices.insert(mesh.indices.end(), { indexOffset + 1, indexOffset + 3, indexOffset, indexOffset + 3, indexOffset + 1, indexOffset + 2 });
        indexOffset += 4;

        mesh.uvs.push_back({ charData.uvTopLeft.x, charData.uvBottomRight.y });
        mesh.uvs.push_back({ charData.uvBottomRight.x, charData.uvBottomRight.y });
        mesh.uvs.push_back({ charData.uvBottomRight.x, charData.uvTopLeft.y });
        mesh.uvs.push_back({ charData.uvTopLeft.x, charData.uvTopLeft.y });
    }

    asset::StaticMesh CreateTextMesh(const TextMeshParams& params)
    {
        asset::StaticMesh mesh = {};
        uint32_t indexOffset = 0;
        const float scale = static_cast<float>(params.fontSize) / asset::builder::FontBlueprint::s_Scale;
        math::Vec2 cursorPos = { };
        cursorPos.y += params.font->GetAscent(params.fontSize);
        if (params.text->empty())
        {
            CreateCharMesh(params.font->GetCharData(' '), cursorPos, mesh, indexOffset, scale);
            return mesh;
        }

        size_t lineStart = 0;
        for (size_t i = 0; i < params.text->size(); ++i)
        {
            const char c = params.text->at(i);
            if (c != '\n')
            {
                const auto& charData = params.font->GetCharData(c);

                if (params.applyKerning)
                {
                    cursorPos = ApplyKerning(cursorPos, i, params.text, charData, scale);
                }
                cursorPos = ApplyLeftSideBearing(cursorPos, charData, scale);
                CreateCharMesh(charData, cursorPos, mesh, indexOffset, scale);
                cursorPos = ApplyAdvanceWidth(cursorPos, charData, scale);
            }

            if (params.wrap == text::WrapMode::Word ||
                params.wrap == text::WrapMode::Char ||
                params.wrap == text::WrapMode::WordChar)
            {
                bool didWrap = false;
                const float oldX = cursorPos.x;
                cursorPos = ApplyWrapping(cursorPos,
                                          c,
                                          params.wrap,
                                          i,
                                          params.rect,
                                          params.font,
                                          params.fontSize,
                                          params.text,
                                          scale,
                                          didWrap);

                if (!didWrap && params.wrap == text::WrapMode::WordChar && cursorPos.x > params.rect->size.x)
                {
                    cursorPos = ApplyWrapping(cursorPos,
                                          c,
                                          text::WrapMode::Char,
                                          i,
                                          params.rect,
                                          params.font,
                                          params.fontSize,
                                          params.text,
                                          scale,
                                          didWrap);
                }

                if (didWrap)
                {
                    const float offset = CalculateJustificationXOffset(params.justification,
                                       oldX,
                                       params.rect);
                    if (offset != 0)
                    {
                        const size_t lineEnd = i;
                        for (size_t j = lineStart * 4; j < lineEnd * 4; ++j)
                        {
                            mesh.vertices[j].x += offset;
                        }
                    }
                    lineStart = i + 1;
                }


                if (!didWrap && i == params.text->size() - 1)
                {
                    const float offset = CalculateJustificationXOffset(params.justification,
                                                               cursorPos.x,
                                                               params.rect);
                    if (offset != 0)
                    {
                        const size_t lineEnd = i + 1;
                        for (size_t j = lineStart * 4; j < lineEnd * 4; ++j)
                        {
                            mesh.vertices[j].x += offset;
                        }
                    }
                }
            }
            else if (i == params.text->size() - 1)
            {
                const float offset = CalculateJustificationXOffset(params.justification,
                                                           cursorPos.x,
                                                           params.rect);
                if (offset != 0)
                {
                    const size_t lineEnd = i + 1;
                    for (size_t j = lineStart * 4; j < lineEnd * 4; ++j)
                    {
                        mesh.vertices[j].x += offset;
                    }
                }
            }
        }


        return mesh;
    }

    asset::StaticMesh CreateTextSelectionMesh(const components::EditableTextComponent& text,
                                              const components::RectTransformComponent& rect)
    {
        auto window = Application::Get()->GetWindow();

        asset::StaticMesh mesh = {};

        float startX = GetCaretPosition(text.selectionStart, text) * window->GetContentScale();
        float endX = GetCaretPosition(text.selectionEnd, text) * window->GetContentScale();

        math::Vec2 TL = { startX, 2.f };
        math::Vec2 BR = { endX, rect.rect.size.y - 4.f };

        mesh.vertices.push_back({ TL.x, BR.y, 0 });
        mesh.vertices.push_back({ BR.x, BR.y, 0 });
        mesh.vertices.push_back({ BR.x, TL.y, 0 });
        mesh.vertices.push_back({ TL.x, TL.y, 0 });

        mesh.indices.insert(mesh.indices.end(), { 1, 3, 0, 3, 1, 2 });

        mesh.uvs.push_back({ 0.f, 0.f});
        mesh.uvs.push_back({ 0.f, 0.f});
        mesh.uvs.push_back({ 0.f, 0.f});
        mesh.uvs.push_back({ 0.f, 0.f});

        return mesh;
    }

    math::Vec2 MeasureText(const Rect* bounds,
                           const asset::Font* font,
                           const int fontSize,
                           const std::string* text,
                           const bool applyKerning,
                           const text::WrapMode wrap)
    {
        return MeasureText(bounds, font, fontSize, text, applyKerning, wrap, text->size());
    }

    math::Vec2 MeasureText(const Rect* bounds,
        const asset::Font* font,
        const int fontSize,
        const std::string* text,
        const bool applyKerning,
        const text::WrapMode wrap,
        const size_t endIndex)
    {
        const float scale = static_cast<float>(fontSize) / asset::builder::FontBlueprint::s_Scale;
        math::Vec2 max = { };

        math::Vec2 cursorPos = { };
        cursorPos.y += font->GetLineHeight(fontSize);
        for (size_t i = 0; i < endIndex; ++i)
        {
            const char c = text->at(i);
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

                if (!didWrap && wrap == text::WrapMode::WordChar && cursorPos.x > bounds->size.x)
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

    size_t GetCharIndexForPosition(const math::Vec2& pos,
        const Rect* bounds,
        const asset::Font* font,
        const int fontSize,
        const std::string* text,
        const bool applyKerning,
        const text::WrapMode wrap,
        const text::Alignment justification)
    {
        const float scale = static_cast<float>(fontSize) / asset::builder::FontBlueprint::s_Scale;
        math::Vec2 cursorPos = { };
        cursorPos.y += font->GetLineHeight(fontSize);
        size_t lineStart = 0;
        std::vector<Rect> boundingBoxes = {};
        for (size_t i = 0; i < text->size(); ++i)
        {
            const char c = text->at(i);
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
                const float oldX = cursorPos.x;
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

                if (!didWrap && wrap == text::WrapMode::WordChar && cursorPos.x > bounds->size.x)
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
                    const float offset = CalculateJustificationXOffset(justification,
                                                               oldX,
                                                               bounds);
                    const size_t lineEnd = i;
                    const float halfFontSize = fontSize * .5f;
                    for (size_t j = lineStart; j < lineEnd; ++j)
                    {
                        const auto& charBounds = boundingBoxes[j];
                        if (std::abs(charBounds.topLeft.x + offset - pos.x) < halfFontSize &&
                            std::abs(charBounds.topLeft.y - pos.y) <= fontSize)
                        {
                            return j;
                        }
                    }

                    lineStart = i + 1;
                }

            }
        }

        const float offset = CalculateJustificationXOffset(justification,
                                                   cursorPos.x,
                                                   bounds);
        const float halfFontSize = fontSize * .5f;
        float minX = std::numeric_limits<float>::max();
        for (size_t j = lineStart; j < text->size(); ++j)
        {
            const auto& charBounds = boundingBoxes[j];
            minX = std::min(minX, charBounds.topLeft.x + offset);
            if (std::abs(charBounds.topLeft.x + offset - pos.x) < halfFontSize &&
                std::abs(charBounds.topLeft.y - pos.y) <= fontSize)
            {
                return j;
            }
        }

        return pos.x < minX ? 0 : text->size();
    }
}
