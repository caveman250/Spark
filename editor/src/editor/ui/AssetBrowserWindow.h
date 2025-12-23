#pragma once

#include "ToolWindow.h"
#include "engine/ecs/ecs_fwd.h"

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

        const ecs::Id& GetWindow() const { return m_Window; }

    private:
        void SetActiveFolder(const std::string& activeFolder);
        void CreatePathBar(const std::shared_ptr<asset::Font>& font);
        void CreatePathItem(ecs::World* world,
                            const std::string& name,
                            const std::string& path,
                            const std::shared_ptr<asset::Font>& font);
        ecs::Id CreateFileItem(ecs::World* world,
                               asset::AssetManager* assetManager,
                               const io::VFSFile& name,
                               const std::shared_ptr<asset::Font>& font);

        ecs::Id m_Window = ecs::s_InvalidEntity;
        ecs::Id m_GridBoxEntity = ecs::s_InvalidEntity;
        ecs::Id m_PathBarBox = ecs::s_InvalidEntity;
        std::vector<ecs::Id> m_PathBarItems = {};
        std::string m_ActiveFolder = "/assets";
        std::shared_ptr<asset::Texture> m_FileTexture = nullptr;
        std::shared_ptr<asset::Texture> m_FolderTexture = nullptr;
    };
}
