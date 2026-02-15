#pragma once
#include "engine/ecs/System.h"
#include "engine/input/Key.h"

namespace se::input
{
    struct InputComponent;
}
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
