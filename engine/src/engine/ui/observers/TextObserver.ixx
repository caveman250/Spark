module;
#include "engine/ecs/Observer.h"

export module TextObserver;
import TextComponent;

namespace se::ui::observers
{
    class TextObserver : public ecs::Observer<components::TextComponent>
    {
        void OnAdded(const ecs::Id& entity, components::TextComponent* component) override;
        void OnRemoved(const ecs::Id& entity, components::TextComponent* component) override;
    };
}
