#include "spark.h"
#include "FBXBlueprint.h"
#include "ofbx.h"
#include "engine/asset/mesh/Model.h"
#include "engine/asset/meta/MetaDataManager.h"
#include "engine/io/VFS.h"
#include "engine/reflect/Util.h"

namespace se::asset::builder
{
    std::regex FBXBlueprint::GetFilePattern() const
    {
        return std::regex(".*.fbx");
    }

    std::vector<BuiltAsset> FBXBlueprint::BuildAsset(const std::string& path, const std::string&) const
    {
        auto metaManager = meta::MetaManager::Get();
        auto metaData = metaManager->GetOrCreateMetaDataForAsset<meta::ModelMetaData>(path);

        ofbx::LoadFlags flags =
                //		ofbx::LoadFlags::IGNORE_MODELS |
                ofbx::LoadFlags::IGNORE_BLEND_SHAPES |
                ofbx::LoadFlags::IGNORE_CAMERAS |
                ofbx::LoadFlags::IGNORE_LIGHTS |
                ofbx::LoadFlags::IGNORE_TEXTURES |
                ofbx::LoadFlags::IGNORE_SKIN |
                ofbx::LoadFlags::IGNORE_BONES |
                ofbx::LoadFlags::IGNORE_PIVOTS |
                ofbx::LoadFlags::IGNORE_MATERIALS |
                ofbx::LoadFlags::IGNORE_POSES |
                ofbx::LoadFlags::IGNORE_VIDEOS |
                ofbx::LoadFlags::IGNORE_LIMBS |
                //		ofbx::LoadFlags::IGNORE_MESHES |
                ofbx::LoadFlags::IGNORE_ANIMATIONS;

        size_t fileSize;
        char* data = io::VFS::Get().ReadBinary(path, fileSize);
        ofbx::IScene* scene = ofbx::load((ofbx::u8*)data, fileSize, (ofbx::u16)flags);
        auto model = Model::FromFBX(scene, metaData);
        model->m_SourcePath = path;
        auto db = reflect::SerialiseType<Model>(model.get());

        std::free(data);
        scene->destroy();

        metaData->SetFormatVersion(GetLatestVersion());
        metaManager->SaveMetaData(metaData);

        return { { db, ""} };
    }

    bool FBXBlueprint::IsOutOfDate(const std::string& assetPath, const std::string& outputPath)
    {
        auto metaManager = meta::MetaManager::Get();
        auto metaData = metaManager->GetOrCreateMetaDataForAsset<meta::ModelMetaData>(assetPath);
        if (metaData->GetFormatVersion() < GetLatestVersion())
        {
            return true;
        }

        return Blueprint::IsOutOfDate(assetPath, outputPath);
    }
}
