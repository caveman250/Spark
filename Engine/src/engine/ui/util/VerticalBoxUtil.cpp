#include "VerticalBoxUtil.h"

#include "engine/Application.h"
#include "engine/ui/components/VerticalBoxComponent.h"

namespace se::ui::util
{
    void AddVerticalBoxChild(const ecs::Id& verticalBoxEntity, components::VerticalBoxComponent* verticalBox, const ecs::Id& child)
    {
        auto world = Application::Get()->GetWorld();
        world->AddChild(verticalBoxEntity, child);
        verticalBox->dirty = true;
    }

    void RemoveVerticalBoxChild(const ecs::Id& verticalBoxEntity, components::VerticalBoxComponent* verticalBox, const ecs::Id& child)
    {
        auto world = Application::Get()->GetWorld();
        world->RemoveChild(verticalBoxEntity, child);
        verticalBox->dirty = true;
    }
}
