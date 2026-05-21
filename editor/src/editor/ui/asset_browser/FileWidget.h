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
        static std::shared_ptr<FileWidget> CreateFileWidget(const io::VFSFile& file, const std::shared_ptr<AssetBrowserWindow>& assetBrowser);

        const ecs::Id& GetId() const { return m_Id; }
        const io::VFSFile& GetFile() const { return m_File; }
        void SetFile(const io::VFSFile& file) { m_File = file; }
    private:
        static void BeginRename(const ecs::Id& labelEntity, const std::string& fileName);
        static void DoRename(const std::string& newName, const io::VFSFile& file);
        void EndRename(const std::string& newName);
        static void DuplicateFile(const io::VFSFile& file, const std::weak_ptr<AssetBrowserWindow>& weakAssetBrowser);
        ecs::Id m_Id = {};
        ecs::Id m_Label = {};
        io::VFSFile m_File = {};

        static asset::AssetReference<asset::Texture> FileTexture;
        static asset::AssetReference<asset::Texture> FolderTexture;
    };
}