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
            { 0.f,                              static_cast<float>(rect.size.y),     0 },
            { static_cast<float>(rect.size.x),    static_cast<float>(rect.size.y),     0 },
            { static_cast<float>(rect.size.x),    0.f,                                 0 },
            { 0.f,                              0.f,                               0 },
        };
        mesh.indices = {0, 3, 1, 2, 1, 3};
        mesh.uvs =
        {
            { 0, 1 },
            { 1, 1 },
            { 1, 0 },
            { 0, 0 }
        };
        return mesh;
    }

    asset::StaticMesh CreateTextMesh(const Rect& rect, const std::shared_ptr<asset::Font> &font, int fontSize, const String& text, bool applyKerning, bool wrap)
    {
        asset::StaticMesh mesh;
        uint32_t indexOffset = 0;
        math::Vec2 cursorPos = { }; // TODO alignment
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

            math::Vec2 TL = charData.rect.topLeft + cursorPos;
            math::Vec2 BR = TL + charData.rect.size;
            mesh.vertices.push_back({ TL.x, BR.y, 0 });
            mesh.vertices.push_back({ BR.x, BR.y, 0 });
            mesh.vertices.push_back({ BR.x, TL.y, 0 });
            mesh.vertices.push_back({ TL.x, TL.y, 0 });

            mesh.indices.insert(mesh.indices.end(), { indexOffset, indexOffset + 3, indexOffset + 1, indexOffset + 2, indexOffset + 1, indexOffset + 3} );
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
                        const auto &nextCharData = font->GetCharData(fontSize, nextChar);

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
}
