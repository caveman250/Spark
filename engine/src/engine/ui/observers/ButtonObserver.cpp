#include "ButtonObserver.h"

#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/asset/shader/Shader.h"
#include "engine/render/Material.h"
#include "engine/render/MaterialInstance.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::observers
{
    void ButtonObserver::OnAdded(const ecs::Id& entity, components::ButtonComponent *component)
    {
        auto world = Application::Get()->GetWorld();

        if (!world->HasComponent<components::ImageComponent>(entity))
        {
            auto image = world->AddComponent<components::ImageComponent>(entity);

            auto alphaTexture = asset::AssetManager::Get()->GetAsset<render::Material>("/engine_assets/materials/ui_alpha_texture.sass");
            image->materialInstance = render::MaterialInstance::CreateMaterialInstance(alphaTexture);
            image->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2DReference, 1, &component->image);
        }

        if (!world->HasComponent<components::MouseInputComponent>(entity))
        {
            world->AddComponent<components::MouseInputComponent>(entity);
        }

        if (!world->HasComponent<components::WidgetComponent>(entity))
        {
            world->AddComponent<components::WidgetComponent>(entity);
        }
    }

    void ButtonObserver::OnRemoved(const ecs::Id&, components::ButtonComponent*)
    {
        // leave image there for time being. see how annoying it is in practice.
    }
}
