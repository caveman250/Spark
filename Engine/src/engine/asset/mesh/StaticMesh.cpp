#include "spark.h"
#include "StaticMesh.h"

namespace se::asset
{
    DEFINE_SPARK_CLASS_BEGIN(StaticMesh)
        DEFINE_SERIALIZED_MEMBER(vertices)
        DEFINE_SERIALIZED_MEMBER(uvs)
        DEFINE_SERIALIZED_MEMBER(normals)
        DEFINE_SERIALIZED_MEMBER(indices)
    DEFINE_SPARK_CLASS_END(StaticMesh)
}