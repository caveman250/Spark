#include "ButtonObserver.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ecs/components/RootComponent.h"
#include "engine/ui/components/ImageComponent.h"

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
            image->material = render::Material::CreateMaterial({vert}, {frag});
            auto texture = assetManager->GetAsset<asset::Texture>("/builtin_assets/textures/default_texture.sass");
            image->material->SetUniform("Image", asset::shader::ast::AstType::Sampler2D, 1, &texture);
        }
    }

    void ButtonObserver::OnRemoved(ecs::Id entity, components::ButtonComponent *component)
    {
        // leave image there for time being. see how annoying it is in practice.
    }
}
