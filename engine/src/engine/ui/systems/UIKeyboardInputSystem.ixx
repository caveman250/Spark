module;
#include "engine/ecs/components/RootComponent.h"
#include "engine/ui/components/KeyInputComponent.h"

export module UIKeyboardInputSystem;
import System;
import TitleBarComponent;

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    export class UIKeyboardInputSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;

    private:
        bool TryConsumeEvent(const input::KeyEvent& keyEvent, components::KeyInputComponent& inputReceiver);
    };
}
