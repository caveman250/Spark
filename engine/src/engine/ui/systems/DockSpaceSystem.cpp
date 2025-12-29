#include "DockSpaceSystem.h"
#include <easy/profiler.h>
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/WidgetComponent.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    ecs::SystemDeclaration DockSpaceSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration();
    }

    void DockSpaceSystem::OnUpdate(const ecs::SystemUpdateData&)
    {
        EASY_BLOCK("DockSpaceSystem::OnUpdate");


    }
}
