#pragma once

#include "engine/ecs/System.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/input/InputComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/TitleBarComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    class UIMouseInputSystem : public ecs::EngineSystem
    {
        SPARK_SYSTEM()
    public:
        static ecs::SystemDeclaration GetSystemDeclaration();
        void OnUpdate(const ecs::SystemUpdateData& updateData) override;
        ecs::UpdateMode GetUpdateMode() const override { return ecs::UpdateMode::SingleThreaded; }

    private:
        bool TryConsumeEvent(const input::MouseEvent& keyEvent, components::MouseInputComponent& inputReceiver);
    };
}
