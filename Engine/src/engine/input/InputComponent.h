#pragma once

#include "KeyEvent.h"
#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::input
{
    struct InputComponent
    {
        DECLARE_SPARK_SINGLETON_COMPONENT(InputComponent)

        std::vector<KeyEvent> keyEvents;
    };
}
