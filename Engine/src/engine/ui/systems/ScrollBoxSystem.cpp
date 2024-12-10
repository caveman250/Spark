#include "spark.h"

#include "engine/math/Mat4.h"
#include "engine/ecs/components/MeshComponent.h"
#include "ScrollBoxSystem.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/profiling/Profiler.h"
#include "engine/render/Renderer.h"
#include "engine/render/VertexBuffer.h"
#include "engine/ui/util/MeshUtil.h"
#include "platform/IWindow.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ui::systems
{
    DEFINE_SPARK_SYSTEM(ScrollBoxSystem)

    void ScrollBoxSystem::OnRender(const std::vector<ecs::Id>& entities, components::ScrollBoxComponent* scrollBoxes)
    {
        PROFILE_SCOPE("ScrollBoxSystem::OnRender")

        auto app = Application::Get();
        auto renderer = render::Renderer::Get();
        auto window = app->GetPrimaryWindow();
        auto assetManager = asset::AssetManager::Get();
        auto windowsSize = math::Vec2(static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()));

        for (size_t i = 0; i < entities.size(); ++i)
        {

        }
    }
}
