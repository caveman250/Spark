#pragma once

#include "spark.h"

namespace se::ui::util
{
    struct ContextMenuParams
    {
        int fontSize = 14;
        int minWidth = 200;
        math::IntVec2 mousePos = {};
        ecs::Id scene = false;
        std::vector<std::pair<std::string, std::function<void()>>> options = {};

        void AddOption(const std::string& name, const std::function<void()>& cb);
    };

    void CreateContextMenu(ContextMenuParams& params);
}