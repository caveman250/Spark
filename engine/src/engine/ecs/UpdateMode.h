#pragma once

namespace se::ecs
{
    SPARK_ENUM()
    enum class UpdateMode
    {
        SingleThreaded,
        MultiThreaded
    };
}