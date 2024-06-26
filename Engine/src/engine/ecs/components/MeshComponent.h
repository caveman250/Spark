#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"
#include "engine/asset/mesh/Model.h"

namespace se::render
{
    class Material;
    class VertexBuffer;
}

namespace se::ecs::components
{
    struct MeshComponent
    {
        DECLARE_SPARK_COMPONENT(MeshComponent)

        MeshComponent();

        std::shared_ptr<se::render::Material> material = {};
        std::shared_ptr<se::render::VertexBuffer> vertBuffer = {};
        se::asset::Model model = {};
    };
}
