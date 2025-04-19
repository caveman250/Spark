#pragma once

#include "spark.h"
#include "engine/asset/font/Font.h"
#include "engine/ecs/Signal.h"
#include "engine/reflect/Reflect.h"
#include "engine/string/String.h"

namespace se::ui::components
{
    struct TreeNodeComponent : reflect::ObjectBase
    {
        DECLARE_SPARK_WIDGET_COMPONENT(TreeNodeComponent)

        bool collapsed = false;
        ecs::Signal<bool> onCollapsedStateChange = {};
        String text = "";
        ecs::Signal<> onSelected = {};
        std::shared_ptr<asset::Font> font = nullptr;
        int fontSize = 16;
        int depth = 0;
    };
}