#include "SplitViewUtil.h"

#include "engine/Application.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/SplitViewComponent.h"
#include "engine/ui/components/WindowComponent.h"
#include "engine/render/Material.h"

namespace se::ui::util
{
    ecs::Id AddSplitView(const ecs::Id& scene,
        const ecs::Id& window1,
        const ecs::Id& window2,
        const SplitViewDirection dir,
        const float sliderVal)
    {
        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();

        ecs::Id splitView = world->CreateEntity(scene, "SplitView");
        auto splitViewComp = world->AddComponent<SplitViewComponent>(splitView);
        splitViewComp->dir = dir;
        splitViewComp->sliderPos = sliderVal;
        world->AddComponent<WidgetComponent>(splitView);
        world->AddComponent<MouseInputComponent>(splitView);
        auto keyInput = world->AddComponent<KeyInputComponent>(splitView);
        keyInput->keyMask = input::Key::Unknown;

        auto rect = world->AddComponent<RectTransformComponent>(splitView);
        rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        rect->needsLayout = true;

        world->AddChild(splitView, window1);
        splitViewComp->entity1 = window1;
        if (world->HasComponent<WindowComponent>(window1))
        {
            auto window = world->GetComponent<WindowComponent>(window1);
            window->docked = true;
        }

        splitViewComp->sliderEntity = world->CreateEntity(scene, "Slider");
        world->AddComponent<RectTransformComponent>(splitViewComp->sliderEntity);
        world->AddComponent<WidgetComponent>(splitViewComp->sliderEntity);
        auto bgMaterial = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_splitview_slider.sass");
        auto sliderImage = world->AddComponent<ImageComponent>(splitViewComp->sliderEntity);
        sliderImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(bgMaterial);
        world->AddChild(splitView, splitViewComp->sliderEntity);

        world->AddChild(splitView, window2);
        splitViewComp->entity2 = window2;
        if (world->HasComponent<WindowComponent>(window2))
        {
            auto window = world->GetComponent<WindowComponent>(window2);
            window->docked = true;
        }

        return splitView;
    }
}
