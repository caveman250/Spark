#pragma once

#include "engine/ecs/System.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ecs/components/RootComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class WidgetVisibilitySystem : public ecs::EngineSystem
    {
    SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;

    private:
        void UpdateWidgetVisibility(const ecs::Id& entity, components::WidgetComponent& widget);
    };
}
