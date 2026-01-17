#include "FPSCounterSystem.h"

#if WITH_DEV_ONLY_CLASSES

#include "engine/asset/AssetManager.h"
#include "engine/debug/components/FPSCounterComponent.h"
#include "engine/ecs/util/SystemUtil.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/systems/RectTransformSystem.h"

namespace se::debug::systems
{
    ecs::SystemDeclaration FPSCounterSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration("FPS Counter System")
                    .WithComponent<ui::components::TextComponent>()
                    .WithComponent<components::FPSCounterComponent>()
                    .WithDependency<ui::systems::RectTransformSystem>();
    }

    void FPSCounterSystem::OnInit(const ecs::SystemUpdateData&)
    {
        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();
        ecs::Id fpsCounterEntity = world->CreateEntity("FPS Counter");
        auto text = world->AddComponent<ui::components::TextComponent>(fpsCounterEntity);
        text->font = assetManager->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
        text->fontSize = 24;
        text->alignment = ui::text::Alignment::Right;
        auto rect = world->AddComponent<ui::components::RectTransformComponent>(fpsCounterEntity);
        rect->anchors = { 1.f, 1.f, 0.f, 0.f };
        rect->minX = 60;
        rect->maxY = 50;
        world->AddComponent<components::FPSCounterComponent>(fpsCounterEntity);
    }

    void FPSCounterSystem::OnUpdate(const ecs::SystemUpdateData& systemUpdateData)
    {
        auto* textComps = systemUpdateData.GetComponentArray<ui::components::TextComponent>();
        auto* fpsCounters = systemUpdateData.GetComponentArray<components::FPSCounterComponent>();

        ecs::util::ForEachEntity(this, systemUpdateData,
        [textComps, fpsCounters] (size_t i)
        {
            auto& text = textComps[i];
            auto& fpsCounter = fpsCounters[i];

            clock_t timeThisFrame = clock();
            clock_t delta = timeThisFrame - fpsCounter.timeLastFrame;
            fpsCounter.timeLastFrame = timeThisFrame;
            constexpr float s_MillisecondsToSeconds = 1000.f;
            double milliseconds = (delta/static_cast<double>(CLOCKS_PER_SEC)) * 1000.0;
            fpsCounter.frameTimeBuffer[fpsCounter.currentFrameIndex] = milliseconds;
            fpsCounter.currentFrameIndex = (fpsCounter.currentFrameIndex + 1) % components::FPSCounterComponent::s_NumFramesToBuffer;

            double average = 0.0;
            for (uint8_t j = 0; j < components::FPSCounterComponent::s_NumFramesToBuffer; ++j)
            {
                average += fpsCounter.frameTimeBuffer[j];
            }
            constexpr float reciprocal = 1.f / components::FPSCounterComponent::s_NumFramesToBuffer;
            average *= reciprocal;

            average = s_MillisecondsToSeconds / average;

            text.text = std::format("{:.0f}", std::max(1.0, average));
        });
    }
}

#endif
