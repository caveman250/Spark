#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/ParentComponent.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/RectTransformComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class ImageRenderSystem : public ecs::EngineSystem<const components::RectTransformComponent, components::ImageComponent>
    {
        DECLARE_SPARK_SYSTEM(ImageRenderSystem)
    public:
        void OnRender(const std::vector<ecs::Id>& entities, const components::RectTransformComponent* transform, components::ImageComponent*) override;
    };
}
