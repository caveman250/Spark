#pragma once

#include "spark.h"
#include "engine/asset/texture/Texture.h"
#include "engine/ecs/Signal.h"
#include "engine/reflect/Object.h"
#include "engine/reflect/Reflect.h"

namespace se::render::commands
{
    class RenderCommand;
}

namespace se::ui::singleton_components
{
    struct UIRenderComponent : reflect::ObjectBase
    {
        SPARK_SINGLETON_COMPONENT(UIRenderComponent)

        std::unordered_map<ecs::Id, std::vector<render::commands::RenderCommand*>> entityPreRenderCommands;
        std::unordered_map<ecs::Id, std::vector<render::commands::RenderCommand*>> entityRenderCommands;
        std::unordered_map<ecs::Id, std::vector<render::commands::RenderCommand*>> entityPostRenderCommands;
    };
}
