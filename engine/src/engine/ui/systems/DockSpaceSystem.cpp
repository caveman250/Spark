#include "DockSpaceSystem.h"
#include "engine/profiling/Profiler.h"
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
        PROFILE_SCOPE("DockSpaceSystem::OnUpdate")


    }
}
