#include "MeshUtil.h"

namespace se::ui::util
{
    asset::StaticMesh MeshUtil::CreateMeshFromRect(const Rect& rect)
    {
        asset::StaticMesh mesh;
        mesh.vertices =
        {
            { rect.topLeft.x, rect.bottomRight.y, 0 },
            { rect.bottomRight.x, rect.bottomRight.y, 0 },
            { rect.bottomRight.x, rect.topLeft.y, 0 },
            { rect.topLeft.x, rect.topLeft.y, 0 },
        };
        mesh.indices = {0, 3, 1, 2, 1, 3};
        mesh.uvs =
        {
            { 0, 0 },
            { 1, 0 },
            { 1, 1 },
            { 0, 1 }
        };
        return mesh;
    }
}
