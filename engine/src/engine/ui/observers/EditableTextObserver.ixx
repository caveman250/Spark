module;
#include "engine/ecs/Observer.h"

export module EditableTextObserver;
import EditableTextComponent;

namespace se::ui::observers
{
    class EditableTextObserver : public ecs::Observer<components::EditableTextComponent>
    {
        void OnAdded(const ecs::Id& entity, components::EditableTextComponent* component) override;
        void OnRemoved(const ecs::Id& entity, components::EditableTextComponent* component) override;
    };
}
