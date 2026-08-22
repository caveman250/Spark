module;

#include "spark.h"


export module ContextMenuUtil;

import System;

namespace se::ui::util
{
    export struct ContextMenuParams
    {
        int fontSize = 14;
        int minWidth = 200;
        math::IntVec2 mousePos = {};
        ecs::Id scene = ecs::InvalidEntity;
        std::vector<std::pair<std::string, std::function<void()>>> options = {};
        ecs::System* system = nullptr;

        void AddOption(const std::string& name, const std::function<void()>& cb);
    };

    void CreateContextMenu(ContextMenuParams& params);
}