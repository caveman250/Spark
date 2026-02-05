#pragma once

#include "spark.h"
#include "engine/ecs/Component.h"
#include "engine/render/MaterialInstance.h"
#include "engine/render/VertexBuffer.h"
#include "engine/render/IndexBuffer.h"
#include "engine/ui/Rect.h"
#include "engine/asset/AssetReference.h"
#include "engine/asset/texture/Texture.h"

namespace se::ui::components
{
    struct ImageComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        SPARK_MEMBER(Serialized)
        asset::AssetReference<asset::Texture> texture = {};

        std::shared_ptr<render::MaterialInstance> materialInstance = {};
        std::shared_ptr<render::VertexBuffer> vertBuffer = {};
        std::shared_ptr<render::IndexBuffer> indexBuffer = {};
        Rect lastRect = {};
    };
}