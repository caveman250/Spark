#pragma once

namespace se::debug::components
{
    struct FPSCounterComponent : ecs::Component
    {
        SPARK_COMPONENT()

        static constexpr size_t s_NumFramesToBuffer = 60;
        clock_t timeLastFrame = 0;
        double frameTimeBuffer[s_NumFramesToBuffer] = { };
        uint8_t currentFrameIndex = 0;
    };
}
