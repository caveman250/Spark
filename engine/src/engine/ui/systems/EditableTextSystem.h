#pragma once
#include "engine/ecs/System.h"
#include "engine/input/Key.h"

namespace se::ui::components
{
    struct KeyInputComponent;
    struct EditableTextComponent;
}

namespace se::ui::systems
{
    class EditableTextSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        void OnUpdate(const ecs::SystemUpdateData&) override;
        void OnRender(const ecs::SystemUpdateData&) override;

    private:
        void HandleKey(const ecs::Id& entity,
                   components::EditableTextComponent& text,
                   components::KeyInputComponent& keyInput,
                   input::Key key);
    };
}
