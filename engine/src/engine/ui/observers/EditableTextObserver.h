#pragma once
#include "engine/ecs/Observer.h"
#include "engine/ui/components/EditableTextComponent.h"

namespace se::ui::observers
{
    class EditableTextObserver : public ecs::Observer<components::EditableTextComponent>
    {
        void OnAdded(ecs::Id entity, components::EditableTextComponent* component) override;
        void OnRemoved(ecs::Id entity, components::EditableTextComponent* component) override;
    };
}
