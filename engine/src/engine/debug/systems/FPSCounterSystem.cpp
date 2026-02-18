#include "FPSCounterSystem.h"

#if WITH_DEV_ONLY_CLASSES

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

    void FPSCounterSystem::OnInit(const ecs::QueryResults&)
    {
        const auto world = Application::Get()->GetWorld();
        const ecs::Id fpsCounterEntity = world->CreateEntity("FPS Counter");
        const auto text = world->AddComponent<ui::components::TextComponent>(fpsCounterEntity);
        text->font = "/engine_assets/fonts/Arial.sass";
        text->fontSize = 24;
        text->alignment = ui::text::Alignment::Right;
        const auto rect = world->AddComponent<ui::components::RectTransformComponent>(fpsCounterEntity);
        rect->anchors = { 1.f, 1.f, 0.f, 0.f };
        rect->minX = 60;
        rect->maxY = 50;
        world->AddComponent<components::FPSCounterComponent>(fpsCounterEntity);
    }

    void FPSCounterSystem::OnUpdate(const ecs::QueryResults& results)
    {
        ecs::ForEachArcheType(results, ecs::UpdateMode::MultiThreaded, false, [](const ecs::SystemUpdateData& updateData)
        {
            const auto& entities = updateData.GetEntities();
            auto* textComps = updateData.GetComponentArray<ui::components::TextComponent>();
            auto* fpsCounters = updateData.GetComponentArray<components::FPSCounterComponent>();

            for (size_t i = 0; i < entities.size(); ++i)
            {
                auto& text = textComps[i];
                auto& fpsCounter = fpsCounters[i];

                const clock_t timeThisFrame = clock();
                const clock_t delta = timeThisFrame - fpsCounter.timeLastFrame;
                fpsCounter.timeLastFrame = timeThisFrame;
                constexpr float s_MillisecondsToSeconds = 1000.f;
                const double milliseconds = (delta/static_cast<double>(CLOCKS_PER_SEC)) * 1000.0;
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
            }
        });
    }
}

#endif
