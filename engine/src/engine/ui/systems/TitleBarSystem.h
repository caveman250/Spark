#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/ParentComponent.h"
#include "engine/input/InputComponent.h"
#include "engine/ui/components/TitleBarComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class TitleBarSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
    };
}
