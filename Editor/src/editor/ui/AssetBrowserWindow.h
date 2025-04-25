#pragma once

#include "ToolWindow.h"
#include "engine/ecs/ecs_fwd.h"
#include "engine/ui/Rect.h"

namespace se::asset
{
    class Texture;
}

namespace se::editor::ui
{
    class AssetBrowserWindow : public ToolWindow
    {
    public:
        AssetBrowserWindow(EditorRuntime* editor) : ToolWindow(editor) {}
        void ConstructUI() override;
        void DestroyUI() override;

    private:
        ecs::Id m_Window;
        std::shared_ptr<asset::Texture> m_TestTexture;
    };
}
