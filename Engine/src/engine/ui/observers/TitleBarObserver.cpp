#include "TitleBarObserver.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/ReceivesMouseEventsComponent.h"
#include "engine/ui/components/TitleBarComponent.h"

namespace se::ui::observers
{
    void TitleBarObserver::OnAdded(ecs::Id entity, components::TitleBarComponent *component)
    {
        auto world = Application::Get()->GetWorld();
        auto* assetManager = asset::AssetManager::Get();

        //background
        if (!world->HasComponent<components::ImageComponent>(entity))
        {
            auto image = world->AddComponent<components::ImageComponent>(entity);

            auto vert = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/ui.sass");
            auto frag = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/flat_color.sass");
            image->material = render::Material::CreateMaterial({vert}, {frag});
            image->material->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.2f, 0.2f, 0.2f));
        }

        if (!world->HasComponent<components::ReceivesMouseEventsComponent>(entity))
        {
            world->AddComponent<components::ReceivesMouseEventsComponent>(entity);
        }
    }

    void TitleBarObserver::OnRemoved(ecs::Id, components::TitleBarComponent*)
    {

    }
}
