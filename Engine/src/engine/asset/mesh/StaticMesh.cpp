#include "spark.h"
#include "StaticMesh.h"

namespace se::asset
{
    DEFINE_SPARK_CLASS_BEGIN(StaticMesh)
        DEFINE_MEMBER(vertices)
        DEFINE_MEMBER(uvs)
        DEFINE_MEMBER(normals)
        DEFINE_MEMBER(indices)
    DEFINE_SPARK_CLASS_END()
}