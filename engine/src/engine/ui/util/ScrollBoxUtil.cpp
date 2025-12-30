#include "ScrollBoxUtil.h"

#include <engine/ui/components/ScrollBoxComponent.h>
#include <engine/ui/components/ScrollViewComponent.h>
#include <engine/asset/AssetManager.h>
#include <engine/ui/components/ImageComponent.h>
#include <engine/asset/shader/Shader.h>
#include <engine/ui/components/RectTransformComponent.h>
#include <engine/ui/components/MouseInputComponent.h>

#include "engine/render/Material.h"
#include "engine/render/MaterialInstance.h"
#include "engine/ui/components/WidgetComponent.h"

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

        auto scrollBoxEntity = world->CreateEntity("ScrollBox", editorOnly);
        *outScrollBox = world->AddComponent<components::ScrollBoxComponent>(scrollBoxEntity);
        *outTransform = world->AddComponent<components::RectTransformComponent>(scrollBoxEntity);
        (*outTransform)->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };

        scrollViewEntity = world->CreateEntity("ScrollView", editorOnly);
        world->AddComponent<components::WidgetComponent>(scrollViewEntity);
        *outScrollView = world->AddComponent<components::ScrollViewComponent>(scrollViewEntity);
        auto scrollViewTransform = world->AddComponent<components::RectTransformComponent>(scrollViewEntity);
        scrollViewTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        scrollViewTransform->overridesChildSizes = true;
        auto scrollViewMouseEventComp = world->AddComponent<components::MouseInputComponent>(scrollViewEntity);
        scrollViewMouseEventComp->receivesScrollEvents = true;
        scrollViewMouseEventComp->buttonMask = 0;

        scrollBarEntity = world->CreateEntity("Scroll Bar", editorOnly);
        auto scrollBarImage = world->AddComponent<components::ImageComponent>(scrollBarEntity);
        static std::shared_ptr<render::Material> material = nullptr;
        if (!material)
        {
            material = std::make_shared<render::Material>(
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/ui.sass") },
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/flat_color.sass") });
            material->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.8f, 0.8f, 0.8f));
        }
        scrollBarImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        auto scrollBarRect = world->AddComponent<components::RectTransformComponent>(scrollBarEntity);
        scrollBarRect->anchors = { 1.f, 1.f, 0.f, 0.f };
        scrollBarRect->minX = 10;
        scrollBarRect->maxX = 5;
        scrollBarRect->minY = 5;
        scrollBarRect->maxY = 25;
        std::function<void(const components::RectTransformComponent*, float)> scrollCb =
            [scrollBarEntity](const components::RectTransformComponent* scrollRect, float scrollAmount)
        {
            auto rect = Application::Get()->GetWorld()->GetComponent<components::RectTransformComponent>(scrollBarEntity);
            float availableSize = scrollRect->rect.size.y - 25.f - 5.f * 2.f;

            rect->minY = 5 + availableSize * scrollAmount;
            rect->maxY = rect->minY + 25;
            rect->needsLayout = true;

            rect->rect.topLeft.y = static_cast<int>(scrollRect->rect.topLeft.y + 5 + availableSize * scrollAmount);
        };
        (*outScrollView)->onScrolled.Subscribe(std::move(scrollCb));

        world->AddChild(scrollBoxEntity, scrollBarEntity);
        world->AddChild(scrollBoxEntity, scrollViewEntity);

        return scrollBoxEntity;
    }
}
