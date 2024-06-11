#include "spark.h"
#include "FBXBlueprint.h"
#include "ofbx.h"
#include "engine/asset/mesh/Model.h"
#include "engine/io/VFS.h"

namespace se::asset::builder
{
    std::regex FBXBlueprint::GetFilePattern() const
    {
        return std::regex(".*.fbx");
    }

    std::shared_ptr<binary::Database> FBXBlueprint::BuildAsset(const std::string& path, asset::meta::MetaData&) const
    {
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
        auto model = Model::FromFBX(scene);
        auto db = model->Serialise();

        std::free(data);
        scene->destroy();

        return db;
    }
}