#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/ParentComponent.h"
#include "engine/input/InputComponent.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/RectTransformComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class ButtonSystem : public ecs::EngineSystem<const components::RectTransformComponent, components::ButtonComponent, components::ImageComponent, input::InputComponent>
    {
        DECLARE_SPARK_SYSTEM(ButtonSystem)
    public:
        void OnUpdate(const std::vector<ecs::Id>& entities, const components::RectTransformComponent*, components::ButtonComponent*, components::ImageComponent*, input::InputComponent*) override;

    private:
        void LayoutChildren(ecs::World* world, ecs::Id entity, const components::RectTransformComponent& parentRect, int depth);
    };
}
