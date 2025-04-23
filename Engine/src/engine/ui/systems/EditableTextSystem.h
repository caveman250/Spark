#pragma once
#include "engine/ecs/System.h"

namespace se::ui::systems
{
    class EditableTextSystem : public ecs::EngineSystem
    {
        DECLARE_SPARK_SYSTEM(EditableTextSystem)

        void OnUpdate(const ecs::SystemUpdateData&) override;
        void OnRender(const ecs::SystemUpdateData&) override;
    };
}
