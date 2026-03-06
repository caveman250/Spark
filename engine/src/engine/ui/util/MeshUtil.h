#pragma once
#include "engine/asset/mesh/StaticMesh.h"
#include "engine/ui/Rect.h"
#include "engine/ui/text/Alignment.h"
#include "engine/ui/text/WrapMode.h"

namespace se::ui::components
{
    struct EditableTextComponent;
}

namespace se::asset
{
    class Font;
}

namespace se::ui::util
{
    asset::StaticMesh CreateMeshFromRect(const Rect& rect);

    struct TextMeshParams
    {
        const Rect* rect = nullptr;
        const asset::Font* font = nullptr;
        int fontSize = {};
        const std::string* text = nullptr;
        bool applyKerning = {};
        text::WrapMode wrap = {};
        text::Alignment justification = {};
    };

    asset::StaticMesh CreateTextMesh(const TextMeshParams& params);
    asset::StaticMesh CreateTextSelectionMesh(const components::EditableTextComponent& text,
                                              const components::RectTransformComponent& rect);

    math::Vec2 MeasureText(const Rect* bounds,
                              const asset::Font* font,
                              int fontSize,
                              const std::string* text,
                              bool applyKerning,
                              text::WrapMode wrap);

    math::Vec2 MeasureText(const Rect* bounds,
                              const asset::Font* font,
                              int fontSize,
                              const std::string* text,
                              bool applyKerning,
                              text::WrapMode wrap,
                              size_t endIndex);

    size_t GetCharIndexForPosition(const math::Vec2& pos,
                                const Rect* bounds,
                                const asset::Font* font,
                                int fontSize,
                                const std::string* text,
                                bool applyKerning,
                                text::WrapMode wrap,
                                text::Alignment justification);
}
