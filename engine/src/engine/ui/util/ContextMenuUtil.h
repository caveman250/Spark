#pragma once

#include "spark.h"

namespace se::ui::util
{
    struct ContextMenuParams
    {
        int fontSize = 14;
        std::vector<std::string> options = {};
        std::function<void(int)> onItemSelected = nullptr;
        math::IntVec2 mousePos = {};
        ecs::Id scene = false;
    };

    void CreateContextMenu(ContextMenuParams& params);
}