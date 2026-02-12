#include "ScrollBoxUtil.h"

#include <engine/ui/components/ScrollBoxComponent.h>
#include <engine/ui/components/ScrollViewComponent.h>
#include <engine/asset/AssetManager.h>
#include <engine/ui/components/ImageComponent.h>
#include <engine/asset/shader/Shader.h>
#include <engine/ui/components/RectTransformComponent.h>
#include <engine/ui/components/MouseInputComponent.h>

#include "engine/io/VFS.h"
#include "engine/render/Material.h"
#include "engine/render/MaterialInstance.h"
#include "engine/ui/components/WidgetComponent.h"

namespace se::ui::util
{
    NewScrollBox CreateScrollBox(const ecs::Id& scene)
    {
        NewScrollBox ret = {};

        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();

        ret.scrollBoxEntity = world->CreateEntity(scene, "ScrollBox");
        ret.scrollBox = world->AddComponent<components::ScrollBoxComponent>(ret.scrollBoxEntity);
        ret.transform = world->AddComponent<components::RectTransformComponent>(ret.scrollBoxEntity);
        ret.transform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };

        ret.scrollViewEntity = world->CreateEntity(scene, "ScrollView");
        world->AddComponent<components::WidgetComponent>(ret.scrollViewEntity);
        ret.scrollView = world->AddComponent<components::ScrollViewComponent>(ret.scrollViewEntity);
        auto scrollViewTransform = world->AddComponent<components::RectTransformComponent>(ret.scrollViewEntity);
        scrollViewTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        auto scrollViewMouseEventComp = world->AddComponent<components::MouseInputComponent>(ret.scrollViewEntity);
        scrollViewMouseEventComp->receivesScrollEvents = true;
        scrollViewMouseEventComp->buttonMask = 0;

        ret.scrollBarEntity = world->CreateEntity(scene, "Scroll Bar");
        auto scrollBarImage = world->AddComponent<components::ImageComponent>(ret.scrollBarEntity);
        std::shared_ptr<render::Material> material = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_scroll_bar.sass");
        scrollBarImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        auto scrollBarRect = world->AddComponent<components::RectTransformComponent>(ret.scrollBarEntity);
        scrollBarRect->anchors = { 1.f, 1.f, 0.f, 0.f };
        scrollBarRect->minX = 10;
        scrollBarRect->maxX = 5;
        scrollBarRect->minY = 5;
        scrollBarRect->maxY = 25;
        std::function scrollCb =
            [scrollBarEntity = ret.scrollBarEntity](const components::RectTransformComponent* scrollRect, float scrollAmount)
        {
            auto rect = Application::Get()->GetWorld()->GetComponent<components::RectTransformComponent>(scrollBarEntity);
            float availableSize = scrollRect->rect.size.y - 25.f - 5.f * 2.f;

            rect->minY = 5 + availableSize * scrollAmount;
            rect->maxY = rect->minY + 25;
            rect->needsLayout = true;

            rect->rect.topLeft.y = static_cast<int>(scrollRect->rect.topLeft.y + 5 + availableSize * scrollAmount);
        };
        ret.scrollView->onScrolled.Subscribe(std::move(scrollCb));

        world->AddChild(ret.scrollBoxEntity, ret.scrollBarEntity);
        world->AddChild(ret.scrollBoxEntity, ret.scrollViewEntity);

        return ret;
    }
}
