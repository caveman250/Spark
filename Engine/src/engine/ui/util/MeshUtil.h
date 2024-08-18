#pragma once
#include "engine/asset/mesh/StaticMesh.h"
#include "engine/ui/Rect.h"

namespace se::ui::util
{
    class MeshUtil 
    {
    public:
        static asset::StaticMesh CreateMeshFromRect(const Rect& rect);
    };
}
