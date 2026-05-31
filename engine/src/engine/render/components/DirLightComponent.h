#pragma once

#include "spark.h"
#include "engine/math/math.h"
#include "engine/ecs/Component.h"
#include "engine/render/IndexBuffer.h"
#include "engine/render/MaterialInstance.h"

namespace se::render::components
{
    struct DirLightComponent : ecs::Component
    {
        SPARK_COMPONENT()

        SPARK_MEMBER(Serialized)
        math::Vec3 color;

#if SPARK_EDITOR
        std::shared_ptr<MaterialInstance> iconMaterial = {};
        std::shared_ptr<VertexBuffer> vertBuffer = {};
        std::shared_ptr<IndexBuffer> indexBuffer = {};
        std::shared_ptr<MaterialInstance> guideMaterial = {};
        std::shared_ptr<VertexBuffer> guideVertBuffer = {};
        std::shared_ptr<IndexBuffer> guideIndexBuffer = {};
#endif
    };
}
