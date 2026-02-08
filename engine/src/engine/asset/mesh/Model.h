#pragma once

#include "spark.h"
#include "StaticMesh.h"
#include "engine/asset/Asset.h"
#include "engine/render/Material.h"
#include "engine/asset/AssetReference.h"

namespace ofbx
{
    struct IScene;
}

namespace se::asset
{
    namespace meta
    {
        class ModelMetaData;
    }

    class Model : public Asset
    {
        SPARK_CLASS()

        std::shared_ptr<meta::MetaData> CreateMetaData() const override;
        bool UsesMetaData() const override { return true; }
        std::string GetSourceFileExtension() const override { return ".fbx"; }

        static std::shared_ptr<Model> FromFBX(ofbx::IScene* fbxScene, const std::shared_ptr<meta::ModelMetaData>& meta);

        const StaticMesh& GetMesh() { return m_Mesh; }
        bool HasMaterial() const { return m_Material.IsSet(); }
        const std::shared_ptr<render::Material>& GetMaterial() { return m_Material.GetAsset(); }
    private:
        SPARK_MEMBER(Serialized)
        StaticMesh m_Mesh;

        SPARK_MEMBER(Serialized)
        asset::AssetReference<render::Material> m_Material;
    };
}
