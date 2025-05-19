#pragma once
#include "engine/asset/mesh/StaticMesh.h"
#include "engine/ui/Rect.h"
#include "engine/ui/text/Alignment.h"
#include "engine/ui/text/WrapMode.h"

namespace se
{
    class String;
}

namespace se::asset
{
    class Font;
}

namespace se::ui::util
{
    asset::StaticMesh CreateMeshFromRect(const Rect& rect);

    asset::StaticMesh CreateTextMesh(const ui::Rect& rect,
                                     const std::shared_ptr<asset::Font>& font,
                                     int fontSize,
                                     const String& text,
                                     bool applyKerning,
                                     text::WrapMode::Type wrap,
                                     text::Alignment::Type justification);

    math::Vec2 MeasureText(const Rect& bounds,
                              const std::shared_ptr<asset::Font>& font,
                              int fontSize,
                              const String& text,
                              bool applyKerning,
                              text::WrapMode::Type wrap);

    math::Vec2 MeasureText(const Rect& bounds,
                              const std::shared_ptr<asset::Font>& font,
                              int fontSize,
                              const String& text,
                              bool applyKerning,
                              text::WrapMode::Type wrap,
                              size_t endIndex);

    int GetCharIndexForPosition(const math::Vec2& pos,
                                const Rect& bounds,
                                const std::shared_ptr<asset::Font>& font,
                                int fontSize,
                                const String& text,
                                bool applyKerning,
                                text::WrapMode::Type wrap,
                                text::Alignment::Type justification);
}
