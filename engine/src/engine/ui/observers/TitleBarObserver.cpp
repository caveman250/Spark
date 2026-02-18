
#include "TitleBarObserver.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/TitleBarComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/render/MaterialInstance.h"

namespace se::ui::observers
{
    void TitleBarObserver::OnAdded(const ecs::Id& entity, components::TitleBarComponent*)
    {
        const auto world = Application::Get()->GetWorld();

        //background
        if (!world->HasComponent<components::ImageComponent>(entity))
        {
            const auto assetManager = asset::AssetManager::Get();
            const auto image = world->AddComponent<components::ImageComponent>(entity);
            const auto material = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_darkbg.sass");
            image->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        }

        if (!world->HasComponent<components::WidgetComponent>(entity))
        {
            world->AddComponent<components::WidgetComponent>(entity);
        }

        if (!world->HasComponent<components::MouseInputComponent>(entity))
        {
            world->AddComponent<components::MouseInputComponent>(entity);
        }
    }

    void TitleBarObserver::OnRemoved(const ecs::Id&, components::TitleBarComponent*)
    {

    }
}
