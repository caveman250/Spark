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

namespace se::render
{
    class VertexBuffer;
    class IndexBuffer;
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

        Model() = default;
        Model(const Model& other);

        std::shared_ptr<meta::MetaData> CreateMetaData() const override;
        bool UsesMetaData() const override { return true; }
        std::string GetSourceFileExtension() const override { return ".fbx"; }

        static std::shared_ptr<Model> FromFBX(const ofbx::IScene* fbxScene, const std::shared_ptr<meta::ModelMetaData>& meta);

        const StaticMesh& GetMesh() { return m_Mesh; }
        bool HasMaterial() const { return m_Material.IsSet(); }
        const std::shared_ptr<render::Material>& GetMaterial() { return m_Material.GetAsset(); }
        const asset::AssetReference<render::Material>& GetMaterialReference() { return m_Material; }

        const std::shared_ptr<render::VertexBuffer>& GetVertexBuffer();
        const std::shared_ptr<render::IndexBuffer>& GetIndexBuffer();

        void SetVertexBuffer(const std::shared_ptr<render::VertexBuffer>& buffer);
        void SetIndexBuffer(const std::shared_ptr<render::IndexBuffer>& buffer);

        void LockBufferMutex(bool locked);
    private:
        SPARK_MEMBER(Serialized)
        StaticMesh m_Mesh;

        SPARK_MEMBER(Serialized)
        AssetReference<render::Material> m_Material;

        std::mutex m_BufferMutex = {};
        std::shared_ptr<render::VertexBuffer> m_VertexBuffer = {};
        std::shared_ptr<render::IndexBuffer> m_IndexBuffer = {};
    };
}
