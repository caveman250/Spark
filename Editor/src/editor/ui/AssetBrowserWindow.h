#pragma once

#include "ToolWindow.h"
#include "engine/ecs/ecs_fwd.h"
#include "engine/ui/Rect.h"

namespace se::ecs
{
    class World;
}

namespace se::asset
{
    class AssetManager;
    class Texture;
    class Font;
}

namespace se::io
{
    struct VFSFile;
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
        void SetActiveFolder(const String& activeFolder);
        ecs::Id CreateFileItem(ecs::World* world,
                               asset::AssetManager* assetManager,
                               const io::VFSFile& name,
                               const std::shared_ptr<asset::Font>& font);

        ecs::Id m_Window;
        ecs::Id m_GridBoxEntity;
        String m_ActiveFolder = "/assets";
        std::shared_ptr<asset::Texture> m_FileTexture = nullptr;
        std::shared_ptr<asset::Texture> m_FolderTexture = nullptr;
    };
}
