#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/TitleBarComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class UIKeyboardInputSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;

    private:
        bool TryConsumeEvent(const input::KeyEvent& keyEvent, components::KeyInputComponent& inputReceiver);
    };
}
