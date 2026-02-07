#pragma once

#include "ToolWindow.h"
#include "engine/ecs/ecs_fwd.h"
#include "engine/asset/AssetReference.h"
#include "engine/asset/texture/Texture.h"
#include "engine/math/IntVec2.h"

namespace se::ecs
{
    class World;
}

namespace se::asset
{
    class AssetManager;
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
        void Update() override;

        const ecs::Id& GetWindow() const { return m_Window; }

    private:
        void SetActiveFolder(const std::string& activeFolder, bool setSelection);
        void CreatePathBar(const asset::AssetReference<asset::Font>& font);
        void CreatePathItem(ecs::World* world,
                            const std::string& name,
                            const std::string& path,
                            const asset::AssetReference<asset::Font>& font);
        ecs::Id CreateFileItem(ecs::World* world,
                               const io::VFSFile& name,
                               const asset::AssetReference<asset::Font>& font);
        void SelectFile(const std::string& file);

        ecs::Id m_Window = ecs::s_InvalidEntity;
        ecs::Id m_GridBoxEntity = ecs::s_InvalidEntity;
        ecs::Id m_GridBG = ecs::s_InvalidEntity;
        ecs::Id m_PathBarBox = ecs::s_InvalidEntity;
        std::vector<ecs::Id> m_PathBarItems = {};
        std::string m_ActiveFolder = "/assets";
        asset::AssetReference<asset::Texture> m_FileTexture = asset::AssetReference<asset::Texture>("/engine_assets/textures/default_file.sass");
        asset::AssetReference<asset::Texture> m_FolderTexture = asset::AssetReference<asset::Texture>("/engine_assets/textures/default_folder.sass");
    };
}
