#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/ParentComponent.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/input/InputComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TitleBarComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class UIKeyboardInputSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;

    private:
        bool TryConsumeEvent(const input::KeyEvent& keyEvent, components::KeyInputComponent& inputReceiver);
    };
}
