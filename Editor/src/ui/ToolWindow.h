#pragma once

namespace se::editor::ui
{
    class ToolWindow 
    {
        virtual void ConstructUI() = 0;
        virtual void DestroyUI() = 0;
    };
}
