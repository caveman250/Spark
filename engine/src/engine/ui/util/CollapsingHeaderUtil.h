#pragma once

#include "engine/ecs/World.h"

namespace se::ui::components
{
    struct CollapsingHeaderComponent;
}

namespace se::ecs
{
    class System;
}

namespace se::ui::util
{
    ecs::Id CreateCollapsingHeader(ecs::World* world,
                                   ecs::Id& titleContainer,
                                   ecs::Id& contents,
                                   components::CollapsingHeaderComponent** collapsingHeader,
                                   bool collapsed,
                                   bool withBackground,
                                   const ecs::Id& scene);
}
