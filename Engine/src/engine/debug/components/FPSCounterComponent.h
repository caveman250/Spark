#pragma once

#include "engine/reflect/Reflect.h"

namespace se::debug::components
{
    struct FPSCounterComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_COMPONENT(FPSCounterComponent)

        static constexpr size_t s_NumFramesToBuffer = 60;
        uint64_t timeLastFrame = 0;
        uint64_t fpsBuffer[s_NumFramesToBuffer] = { };
        uint8_t currentFrameIndex = 0;
    };
}
