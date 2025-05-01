#include "ScrollBoxUtil.h"

#include <engine/ui/components/ScrollBoxComponent.h>
#include <engine/ui/components/ScrollViewComponent.h>
#include <engine/asset/AssetManager.h>
#include <engine/ui/components/ImageComponent.h>
#include <engine/asset/shader/Shader.h>
#include <engine/ui/components/RectTransformComponent.h>
#include <engine/ui/components/MouseInputComponent.h>
#include "engine/render/MaterialInstance.h"

namespace se::ui::util
{
    ecs::Id CreateScrollBox(components::ScrollBoxComponent** outScrollBox,
                            ecs::Id& scrollViewEntity,
                            components::ScrollViewComponent** outScrollView,
                            components::RectTransformComponent** outTransform,
                            ecs::Id& scrollBarEntity,
                            bool editorOnly)
    {
        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();

        auto scrollBoxEntity = world->CreateEntity("ScrollBox", editorOnly);
        *outScrollBox = world->AddComponent<ui::components::ScrollBoxComponent>(scrollBoxEntity);
        *outTransform = world->AddComponent<ui::components::RectTransformComponent>(scrollBoxEntity);
        (*outTransform)->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };

        scrollViewEntity = world->CreateEntity("ScrollView", editorOnly);
        *outScrollView = world->AddComponent<ui::components::ScrollViewComponent>(scrollViewEntity);
        auto scrollViewTransform = world->AddComponent<ui::components::RectTransformComponent>(scrollViewEntity);
        scrollViewTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        scrollViewTransform->overridesChildSizes = true;
        auto scrollViewMouseEventComp = world->AddComponent<components::MouseInputComponent>(scrollViewEntity);
        scrollViewMouseEventComp->receivesScrollEvents = true;

        scrollBarEntity = world->CreateEntity("Scroll Bar", editorOnly);
        auto scrollBarImage = world->AddComponent<ui::components::ImageComponent>(scrollBarEntity);
        auto vert = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/ui.sass");
        auto frag = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/flat_color.sass");
        static std::shared_ptr<render::Material> material = nullptr;
        if (!material)
        {
            material = render::Material::CreateMaterial({vert}, {frag});
            material->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.8f, 0.8f, 0.8f));
        }
        scrollBarImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        auto scrollBarRect = world->AddComponent<ui::components::RectTransformComponent>(scrollBarEntity);
        scrollBarRect->anchors = { 1.f, 1.f, 0.f, 0.f };
        scrollBarRect->minX = 10;
        scrollBarRect->maxX = 5;
        scrollBarRect->minY = 5;
        scrollBarRect->maxY = 25;
        std::function<void(const ui::components::RectTransformComponent*, float)> scrollCb =
            [scrollBarEntity](const ui::components::RectTransformComponent* scrollRect, float scrollAmount)
        {
            auto rect = Application::Get()->GetWorld()->GetComponent<ui::components::RectTransformComponent>(scrollBarEntity);
            float availableSize = scrollRect->rect.size.y - 25.f - 5.f * 2.f;

            rect->minY = static_cast<int>(5 + availableSize * scrollAmount);
            rect->maxY = static_cast<int>(rect->minY + 25);
            rect->needsLayout = true;

            rect->rect.topLeft.y = static_cast<int>(scrollRect->rect.topLeft.y + 5 + availableSize * scrollAmount);
        };
        (*outScrollView)->onScrolled.Subscribe(std::move(scrollCb));

        world->AddChild(scrollBoxEntity, scrollBarEntity);
        world->AddChild(scrollBoxEntity, scrollViewEntity);

        return scrollBoxEntity;
    }
}
