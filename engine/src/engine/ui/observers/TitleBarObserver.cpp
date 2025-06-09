
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
        auto world = Application::Get()->GetWorld();
        auto* assetManager = asset::AssetManager::Get();

        //background
        if (!world->HasComponent<components::ImageComponent>(entity))
        {
            auto image = world->AddComponent<components::ImageComponent>(entity);
            static std::shared_ptr<render::Material> material = nullptr;
            if (!material)
            {
                auto vert = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/ui.sass");
                auto frag = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/flat_color.sass");
                material = render::Material::CreateMaterial({vert}, {frag});
                material->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.2f, 0.2f, 0.2f));
            }
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
