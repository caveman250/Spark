#pragma once

#include "spark.h"
#include "engine/asset/texture/Texture.h"
#include "engine/reflect/Reflect.h"
#include "engine/render/VertexBuffer.h"
#include "engine/render/IndexBuffer.h"
#include "engine/render/Material.h"
#include "engine/ui/Rect.h"

namespace se::ui::components
{
    struct ImageComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_WIDGET_COMPONENT(ImageComponent)

        std::shared_ptr<render::MaterialInstance> materialInstance = {};
        std::shared_ptr<render::VertexBuffer> vertBuffer = {};
        std::shared_ptr<render::IndexBuffer> indexBuffer = {};
        Rect lastRect = {};
    };
}