#pragma once

#include "spark.h"
#include "StaticMesh.h"
#include "engine/asset/Asset.h"

namespace ofbx
{
    struct IScene;
}

namespace se::asset
{
    class Model : public Asset
    {
        SPARK_CLASS(Model)

        static std::shared_ptr<Model> FromFBX(ofbx::IScene* fbxScene);

        const StaticMesh& GetMesh(size_t i) { return m_Meshes[i]; }
    private:
        SPARK_MEMBER(Serialized)
        std::vector<StaticMesh> m_Meshes;
    };
}
