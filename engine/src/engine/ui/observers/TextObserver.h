#pragma once
#include "engine/ecs/Observer.h"
#include "engine/ui/components/TextComponent.h"

namespace se::ui::observers
{
    class TextObserver : public ecs::Observer<components::TextComponent>
    {
        void OnAdded(ecs::Id entity, components::TextComponent* component) override;
        void OnRemoved(ecs::Id entity, components::TextComponent* component) override;
    };
}
