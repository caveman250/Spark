#pragma once
#include "engine/asset/mesh/StaticMesh.h"
#include "engine/ui/Rect.h"

namespace se::asset
{
    class Font;
}

namespace se::ui::util
{
    class MeshUtil 
    {
    public:
        static asset::StaticMesh CreateMeshFromRect(const Rect& rect);
        static asset::StaticMesh CreateTextMesh(const ui::Rect& rect, const std::shared_ptr<asset::Font>& font, int fontSize, const std::string& text);
    };
}