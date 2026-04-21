#pragma once
#include "engine/ecs/Component.h"

namespace se::debug::components
{
    struct FPSCounterComponent : ecs::Component
    {
        SPARK_COMPONENT()

        static constexpr size_t s_NumFramesToBuffer = 60;
        double frameTimeBuffer[s_NumFramesToBuffer] = { };
        uint8_t currentFrameIndex = 0;
    };
}
