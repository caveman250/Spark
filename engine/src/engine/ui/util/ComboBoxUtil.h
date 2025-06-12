#pragma once

#include "engine/ecs/World.h"

namespace se::ui::components
{
    struct ComboBoxComponent;
}

namespace se::ecs
{
    class System;
}

namespace se::ui::util
{
    struct NewComboBox
    {
        ecs::Id id;
        ComboBoxComponent* comboBox;
        RectTransformComponent* rectTransform;
    };

    struct ComboBoxParams
    {
        int fontSize = 14;
        std::vector<String> options = {};
        std::function<void(int)> onItemSelected = nullptr;
        int selectedIndex = 0;
        bool editorOnly = false;
    };

    NewComboBox CreateComboBox(ComboBoxParams& params);
}
