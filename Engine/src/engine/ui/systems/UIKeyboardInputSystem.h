#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/ParentComponent.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/input/InputComponent.h"
#include "engine/ui/components/ReceivesKeyboardEventsComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TitleBarComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class UIKeyboardInputSystem : public ecs::EngineSystem<const components::RectTransformComponent, const RootComponent, components::ReceivesKeyboardEventsComponent, input::InputComponent>
    {
        DECLARE_SPARK_SYSTEM(UIKeyboardInputSystem)
    public:
        void OnUpdate(const std::vector<ecs::Id> &, const components::RectTransformComponent *, const RootComponent *, components::ReceivesKeyboardEventsComponent *, input::InputComponent *) override;

    private:
        bool TryConsumeEvent(const input::KeyEvent& keyEvent, components::ReceivesKeyboardEventsComponent& inputReceiver);
    };
}
