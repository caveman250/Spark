#pragma once

#include "spark.h"
#include "engine/asset/AssetBase.h"
#include "StaticMesh.h"

namespace ofbx
{
    struct IScene;
}

namespace se::asset
{
    class Model : public AssetBase
    {
        DECLARE_SPARK_CLASS(Model)

        std::shared_ptr<binary::Database> Serialise() override;
        void Deserialise(const std::shared_ptr<binary::Database> &db) override;
        static std::shared_ptr<Model> FromFBX(ofbx::IScene* fbxScene);

        const StaticMesh& GetMesh(size_t i) { return m_Meshes[i]; }
    private:
        std::vector<StaticMesh> m_Meshes;
    };
}
