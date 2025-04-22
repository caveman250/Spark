#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/ParentComponent.h"
#include "engine/ui/components/RectTransformComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class RectTransformSystem : public ecs::EngineSystem
    {
    DECLARE_SPARK_SYSTEM(RectTransformSystem)
    public:
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;

    private:
        void LayoutChildren(ecs::World* world, ecs::Id entity, const components::RectTransformComponent& parentRect, int depth);
    };
}
