#pragma once

#include "spark.h"
#include "engine/asset/AssetReference.h"
#include "engine/asset/texture/Texture.h"
#include "engine/io/VFS.h"

namespace se::editor::ui::asset_browser
{
    class AssetBrowserWindow;

    class FileWidget
    {
    public:
        static std::shared_ptr<FileWidget> CreateFileWidget(const io::VFSFile& file, AssetBrowserWindow* assetBrowser);

        const ecs::Id& GetId() const { return m_Id; }
        const io::VFSFile& GetFile() const { return m_File; }
        void SetFile(const io::VFSFile& file) { m_File = file; }
    private:
        static void RenameFile(const std::string& newName, const io::VFSFile& file);
        void OnRename(const std::string& newName);
        ecs::Id m_Id = {};
        ecs::Id m_Label = {};
        io::VFSFile m_File = {};

        static asset::AssetReference<asset::Texture> FileTexture;
        static asset::AssetReference<asset::Texture> FolderTexture;
    };
}