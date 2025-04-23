#pragma once
#include "engine/asset/mesh/StaticMesh.h"
#include "engine/ui/Rect.h"

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
                                     bool wrap);

    math::IntVec2 MeasureText(const Rect& bounds,
                              const std::shared_ptr<asset::Font>& font,
                              int fontSize,
                              const String& text,
                              bool applyKerning,
                              bool wrap);

    math::IntVec2 MeasureText(const Rect& bounds,
                              const std::shared_ptr<asset::Font>& font,
                              int fontSize,
                              const String& text,
                              bool applyKerning,
                              bool wrap,
                              size_t endIndex);

    int GetCharIndexForPosition(const math::Vec2& pos,
                                const Rect& bounds,
                                const std::shared_ptr<asset::Font>& font,
                                int fontSize,
                                const String& text,
                                bool applyKerning,
                                bool wrap);
}
