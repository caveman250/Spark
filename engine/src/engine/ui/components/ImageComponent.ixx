module;

#include "spark.h"
#include "engine/ecs/Component.h"

export module ImageComponent;
import AssetReference;
import MaterialInstance;
import Spark.UI.Rect;

namespace se::ui::components
{
    export struct ImageComponent : ecs::Component
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