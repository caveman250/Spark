#include "engine/render/VertexBuffer.h"
#include "spark.h"
#include "MeshComponent.h"
#include "engine/asset/mesh/Model.h"
#include "engine/render/IndexBuffer.h"
#include "engine/render/Material.h"

namespace se::ecs::components
{
    DEFINE_SPARK_COMPONENT_BEGIN(MeshComponent)
    DEFINE_SPARK_COMPONENT_END(MeshComponent)

    MeshComponent::MeshComponent()
        : ObjectBase()
    {

    }
}