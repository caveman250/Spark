#include "ScrollBoxUtil.h"

#include <engine/ui/components/ScrollBoxComponent.h>
#include <engine/asset/AssetManager.h>
#include <engine/ui/components/ImageComponent.h>
#include <engine/asset/shader/Shader.h>
#include <engine/ui/components/RectTransformComponent.h>

namespace se::ui::util
{
    ecs::Id CreateScrollBox(components::ScrollBoxComponent** outScrollBox, components::RectTransformComponent** outTransform, bool editorOnly)
    {
        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();

        auto scrollBoxEntity = world->CreateEntity("ScrollBox", editorOnly);
        *outScrollBox = world->AddComponent<ui::components::ScrollBoxComponent>(scrollBoxEntity);
        *outTransform = world->AddComponent<ui::components::RectTransformComponent>(scrollBoxEntity);
        (*outTransform)->anchors = { 0.f, 1.f, 0.f, 1.f };

        auto scrollBarEntity = world->CreateEntity("Scroll Bar", editorOnly);
        auto scrollBarImage = world->AddComponent<ui::components::ImageComponent>(scrollBarEntity);
        auto vert = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/ui.sass");
        auto frag = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/flat_color.sass");
        scrollBarImage->material = render::Material::CreateMaterial({vert}, {frag});
        scrollBarImage->material->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.8f, 0.8f, 0.8f));
        auto scrollBarRect = world->AddComponent<ui::components::RectTransformComponent>(scrollBarEntity);
        scrollBarRect->anchors = { 1.f, 1.f, 0.f, 0.f };
        scrollBarRect->minX = -10;
        scrollBarRect->maxX = 5;
        scrollBarRect->minY = 5;
        scrollBarRect->maxY = 25;
        std::function<void(const ui::components::RectTransformComponent*, float, ui::components::RectTransformComponent*)> scrollCb =
            [](const ui::components::RectTransformComponent* scrollRect, float scrollAmount, ui::components::RectTransformComponent* rect)
        {
            float availableSize = scrollRect->rect.size.y - 25.f - 5.f * 2.f;

            rect->minY = static_cast<int>(5 + availableSize * scrollAmount);
            rect->maxY = static_cast<int>(rect->minY + 25);
            rect->needsLayout = true;

            rect->rect.topLeft.y = static_cast<int>(scrollRect->rect.topLeft.y + 5 + availableSize * scrollAmount);
        };
        (*outScrollBox)->onScrolled.Subscribe<ui::components::RectTransformComponent>(scrollBarEntity, std::move(scrollCb));

        world->AddChild(scrollBoxEntity, scrollBarEntity);

        return scrollBoxEntity;
    }
}
