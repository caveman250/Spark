module;

#include "engine/input/Key.h"
#include "engine/ui/components/KeyInputComponent.h"

export module EditableTextSystem;
import System;

namespace se::ui::systems
{
    class EditableTextSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()

        static ecs::SystemDeclaration GetSystemDeclaration();

        void OnUpdate(const ecs::QueryResults&) override;
        void OnRender(const ecs::QueryResults&) override;

    private:
        void HandleKey(const ecs::Id& entity,
                   components::EditableTextComponent& text,
                   components::KeyInputComponent& keyInput,
                   input::Key key,
                   const input::InputComponent* inputComponent);
    };
}
