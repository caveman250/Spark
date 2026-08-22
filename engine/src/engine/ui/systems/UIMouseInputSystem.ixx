module;
#include "engine/ecs/components/RootComponent.h"
#include "engine/input/InputComponent.h"
#include "engine/ui/components/MouseInputComponent.h"

export module UIMouseInputSystem;

import System;
import TitleBarComponent;

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    export class UIMouseInputSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::QueryResults&) override;

    private:
        static bool TryConsumeEvent(const input::MouseEvent& mouseEvent, components::MouseInputComponent& inputReceiver);
    };
}
