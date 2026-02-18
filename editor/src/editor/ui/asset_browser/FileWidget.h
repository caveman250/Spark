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
        static FileWidget* CreateFileWidget(const io::VFSFile& file, AssetBrowserWindow* assetBrowser);

        const ecs::Id& GetId() const { return m_Id; }
        const io::VFSFile& GetFile() const { return m_File; }
        void SetFile(const io::VFSFile& file) { m_File = file; }
    private:
        ecs::Id m_Id = {};
        io::VFSFile m_File = {};

        static asset::AssetReference<asset::Texture> FileTexture;
        static asset::AssetReference<asset::Texture> FolderTexture;
    };
}