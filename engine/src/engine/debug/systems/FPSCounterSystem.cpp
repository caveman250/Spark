#include "FPSCounterSystem.h"

#include "engine/asset/AssetManager.h"
#include "engine/debug/components/FPSCounterComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/systems/RectTransformSystem.h"

namespace se::debug::systems
{
    ecs::SystemDeclaration FPSCounterSystem::GetSystemDeclaration()
    {
        // TODO INSTANTIATE IN DIST ONLY
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
        auto rect = world->AddComponent<ui::components::RectTransformComponent>(fpsCounterEntity);
        rect->anchors = { 1.f, 1.f, 0.f, 0.f };
        rect->minX = 50;
        rect->maxY = 50;
        world->AddComponent<components::FPSCounterComponent>(fpsCounterEntity);
    }

    void FPSCounterSystem::OnUpdate(const ecs::SystemUpdateData& systemUpdateData)
    {
        const auto& entities = systemUpdateData.GetEntities();
        auto* textComps = systemUpdateData.GetComponentArray<ui::components::TextComponent>();
        auto* fpsCounters = systemUpdateData.GetComponentArray<components::FPSCounterComponent>();

        for (size_t i = 0; i < entities.size(); ++i)
        {
            auto& text = textComps[i];
            auto& fpsCounter = fpsCounters[i];

            uint64_t timeThisFrame = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            uint64_t delta = timeThisFrame - fpsCounter.timeLastFrame;
            fpsCounter.timeLastFrame = timeThisFrame;
            constexpr float s_MillisecondsToSeconds = 1000.f;
            fpsCounter.fpsBuffer[fpsCounter.currentFrameIndex] = static_cast<uint64_t>(s_MillisecondsToSeconds / delta);
            fpsCounter.currentFrameIndex = (fpsCounter.currentFrameIndex + 1) % components::FPSCounterComponent::s_NumFramesToBuffer;

            float average = 0.f;
            for (uint8_t j = 0; j < components::FPSCounterComponent::s_NumFramesToBuffer; ++j)
            {
                average += fpsCounter.fpsBuffer[j];
            }
            average /= components::FPSCounterComponent::s_NumFramesToBuffer;

            text.text = std::format("{:.0f}", average);
        }

    }
}
