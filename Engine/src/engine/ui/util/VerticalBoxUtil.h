#pragma once
#include "engine/ecs/ecs_fwd.h"

namespace se::ui::components
{
    struct VerticalBoxComponent;
}

namespace se::ui::util
{
    void AddVerticalBoxChild(const ecs::Id& verticalBoxEntity, components::VerticalBoxComponent* verticalBox, const ecs::Id& child);
    void RemoveVerticalBoxChild(const ecs::Id& verticalBoxEntity, components::VerticalBoxComponent* verticalBox, const ecs::Id& child);
}
