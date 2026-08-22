module;

#include "spark.h"
#include "engine/ecs/Component.h"

export module TreeNodeComponent;
import Signal;
import Font;

namespace se::ui::components
{
    export struct TreeNodeComponent : ecs::Component
    {
        SPARK_WIDGET_COMPONENT()

        bool collapsed = false;
        ecs::Signal<bool> onCollapsedStateChange = {};
        std::string text = "";
        ecs::Signal<> onSelected = {};
        std::shared_ptr<asset::Font> font = nullptr;
        int fontSize = 16;
        int depth = 0;
        std::vector<std::pair<std::string, std::function<void()>>> contextOptions = {};
    };
}