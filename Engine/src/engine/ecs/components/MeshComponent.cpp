#include "engine/render/VertexBuffer.h"
#include "spark.h"
#include "MeshComponent.h"
#include "engine/asset/mesh/Model.h"

namespace se::ecs::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(MeshComponent)
    DEFINE_SPARK_COMPONENT_END()

    MeshComponent::MeshComponent()
        : ObjectBase()
    {

    }
}