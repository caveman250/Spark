#pragma once

#include "spark.h"
#include "engine/asset/font/Font.h"
#include "engine/ecs/Signal.h"

namespace se::ui::components
{
    struct TreeNodeComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        bool collapsed = false;
        ecs::Signal<bool> onCollapsedStateChange = {};
        std::string text = "";
        ecs::Signal<> onSelected = {};
        std::shared_ptr<asset::Font> font = nullptr;
        int fontSize = 16;
        int depth = 0;
        std::vector<std::string> contextOptions = {};
        std::function<void(int)> onContextMenuOptionSelected = nullptr;
    };
}