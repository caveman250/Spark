
#include "ButtonObserver.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/ReceivesMouseEventsComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::observers
{
    void ButtonObserver::OnAdded(ecs::Id entity, components::ButtonComponent *component)
    {
        auto world = Application::Get()->GetWorld();
        auto* assetManager = asset::AssetManager::Get();

        if (!world->HasComponent<components::ImageComponent>(entity))
        {
            auto image = world->AddComponent<components::ImageComponent>(entity);

            auto vert = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/ui.sass");
            auto frag = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/diffuse_texture.sass");
            static auto material = render::Material::CreateMaterial({vert}, {frag}); // TODO

            image->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
            image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &component->image);
        }

        if (!world->HasComponent<ui::components::ReceivesMouseEventsComponent>(entity))
        {
            world->AddComponent<components::ReceivesMouseEventsComponent>(entity);
        }

        if (!world->HasComponent<ui::components::WidgetComponent>(entity))
        {
            world->AddComponent<components::WidgetComponent>(entity);
        }
    }

    void ButtonObserver::OnRemoved(ecs::Id, components::ButtonComponent*)
    {
        // leave image there for time being. see how annoying it is in practice.
    }
}
