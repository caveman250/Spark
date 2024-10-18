#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/ParentComponent.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/input/InputComponent.h"
#include "engine/ui/components/ReceivesMouseEventsComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TitleBarComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class UIMouseInputSystem : public ecs::EngineSystem<const RootComponent, components::ReceivesMouseEventsComponent, input::InputComponent>
    {
        DECLARE_SPARK_SYSTEM(UIMouseInputSystem)
    public:
        void OnUpdate(const std::vector<ecs::Id> &, const RootComponent *, components::ReceivesMouseEventsComponent *, input::InputComponent *) override;

    private:
        bool TryConsumeEvent(const input::MouseEvent& keyEvent, components::ReceivesMouseEventsComponent& inputReceiver);
    };
}
