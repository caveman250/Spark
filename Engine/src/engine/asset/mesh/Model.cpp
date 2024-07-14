#include "spark.h"
#include "Model.h"
#include "ofbx.h"
#include "engine/asset/binary/Database.h"

namespace se::asset
{
    DEFINE_SPARK_CLASS_BEGIN(Model)
        DEFINE_MEMBER(m_Meshes)
    DEFINE_SPARK_CLASS_END()

    std::shared_ptr<binary::Database> Model::Serialise()
    {
        auto db = binary::Database::Create(false);
        binary::StructLayout rootStructLayout =
        {
            { binary::CreateFixedString32("meshes"), binary::Type::Array }
        };

        auto rootStructIndex = db->GetOrCreateStruct(rootStructLayout);
        db->SetRootStruct(rootStructIndex);

        auto root = db->GetRoot();

        binary::StructLayout vec3ObjLayout = //TODO this is crap, need to make a way of storing primitive arrays
        {
            {binary::CreateFixedString32("val"), binary::Type::Vec3}
        };

        binary::StructLayout vec2ObjLayout =
        {
            {binary::CreateFixedString32("val"), binary::Type::Vec2}
        };

        binary::StructLayout uintObjLayout =
        {
            {binary::CreateFixedString32("val"), binary::Type::Uint32}
        };

        binary::StructLayout meshStructLayout =
        {
            {binary::CreateFixedString32("vertices"), binary::Type::Array},
            {binary::CreateFixedString32("normals"), binary::Type::Array},
            {binary::CreateFixedString32("uvs"), binary::Type::Array},
            {binary::CreateFixedString32("indices"), binary::Type::Array}
        };

        auto vec3StructIndex = db->GetOrCreateStruct(vec3ObjLayout);
        auto vec2StructIndex = db->GetOrCreateStruct(vec2ObjLayout);
        auto uintStructIndex = db->GetOrCreateStruct(uintObjLayout);
        auto mipStructIndex = db->GetOrCreateStruct(meshStructLayout);

        auto meshArray = db->CreateArray(mipStructIndex, m_Meshes.size());
        root.Set<binary::Array>("meshes", meshArray);
        for (size_t i = 0; i < m_Meshes.size(); ++i)
        {
            const auto& mesh = m_Meshes[i];
            auto meshObj = meshArray.Get(i);

            auto vertArray = db->CreateArray(vec3StructIndex, mesh.vertices.size());
            for (size_t j = 0; j < mesh.vertices.size(); ++j)
            {
                const auto& vert = mesh.vertices[j];
                auto vertObj = vertArray.Get(j);
                vertObj.Set("val", vert);
            }
            meshObj.Set("vertices", vertArray);

            auto normalArray = db->CreateArray(vec3StructIndex, mesh.normals.size());
            for (size_t j = 0; j < mesh.normals.size(); ++j)
            {
                const auto& normal = mesh.normals[j];
                auto normalObj = normalArray.Get(j);
                normalObj.Set("val", normal);
            }
            meshObj.Set("normals", normalArray);

            auto uvArray = db->CreateArray(vec2StructIndex, mesh.uvs.size());
            for (size_t j = 0; j < mesh.uvs.size(); ++j)
            {
                const auto& uv = mesh.uvs[j];
                auto uvObj = uvArray.Get(j);
                uvObj.Set("val", uv);
            }
            meshObj.Set("uvs", uvArray);

            auto indicesArray = db->CreateArray(uintStructIndex, mesh.indices.size());
            for (size_t j = 0; j < mesh.indices.size(); ++j)
            {
                const auto& index = mesh.indices[j];
                auto indexObj = indicesArray.Get(j);
                indexObj.Set("val", index);
            }
            meshObj.Set("indices", indicesArray);
        }

        return db;
    }

    void Model::Deserialise(const std::shared_ptr<binary::Database>& db)
    {
        auto root = db->GetRoot();
        auto meshArray = root.Get<binary::Array>("meshes");
        for (size_t i = 0; i < meshArray.GetCount(); ++i)
        {
            auto meshObj = meshArray.Get(i);
            auto& mesh = m_Meshes.emplace_back();

            auto vertArray = meshObj.Get<binary::Array>("vertices");
            mesh.vertices.reserve(vertArray.GetCount());
            for (size_t j = 0; j < vertArray.GetCount(); ++j)
            {
                mesh.vertices.push_back(vertArray.Get(j).Get<math::Vec3>("val"));
            }

            auto normalArray = meshObj.Get<binary::Array>("normals");
            mesh.normals.reserve(normalArray.GetCount());
            for (size_t j = 0; j < normalArray.GetCount(); ++j)
            {
                mesh.normals.push_back(normalArray.Get(j).Get<math::Vec3>("val"));
            }

            auto uvsArray = meshObj.Get<binary::Array>("uvs");
            mesh.uvs.reserve(uvsArray.GetCount());
            for (size_t j = 0; j < uvsArray.GetCount(); ++j)
            {
                mesh.uvs.push_back(uvsArray.Get(j).Get<math::Vec2>("val"));
            }

            auto indicesArray = meshObj.Get<binary::Array>("indices");
            mesh.indices.reserve(indicesArray.GetCount());
            for (size_t j = 0; j < indicesArray.GetCount(); ++j)
            {
                mesh.indices.push_back(indicesArray.Get(j).Get<uint32_t>("val"));
            }
        }
    }

    std::shared_ptr<Model> Model::FromFBX(ofbx::IScene* fbxScene)
    {
        auto ret = std::make_shared<Model>();

        int indices_offset = 0;
        int mesh_count = fbxScene->getMeshCount();

        // output unindexed geometry
        for (int mesh_idx = 0; mesh_idx < mesh_count; ++mesh_idx)
        {
            const ofbx::Mesh& mesh = *fbxScene->getMesh(mesh_idx);
            const ofbx::GeometryData& geom = mesh.getGeometryData();
            const ofbx::Vec3Attributes positions = geom.getPositions();
            const ofbx::Vec3Attributes normals = geom.getNormals();
            const ofbx::Vec2Attributes uvs = geom.getUVs();

            for (int partition_idx = 0; partition_idx < geom.getPartitionCount(); ++partition_idx)
            {
                StaticMesh& staticMesh = ret->m_Meshes.emplace_back();
                const ofbx::GeometryPartition& partition = geom.getPartition(partition_idx);
                for (int polygon_idx = 0; polygon_idx < partition.polygon_count; ++polygon_idx)
                {
                    const ofbx::GeometryPartition::Polygon& polygon = partition.polygons[polygon_idx];

                    for (int i = polygon.from_vertex; i < polygon.from_vertex + polygon.vertex_count; ++i)
                    {
                        ofbx::Vec3 v = positions.get(i);
                        staticMesh.vertices.emplace_back(v.x, v.y, v.z);
                    }

                    bool has_normals = normals.values != nullptr;
                    if (has_normals)
                    {
                        for (int i = polygon.from_vertex; i < polygon.from_vertex + polygon.vertex_count; ++i)
                        {
                            ofbx::Vec3 n = normals.get(i);
                            staticMesh.normals.emplace_back(n.x, n.y, n.z);
                        }
                    }

                    bool has_uvs = uvs.values != nullptr;
                    if (has_uvs)
                    {
                        for (int i = polygon.from_vertex; i < polygon.from_vertex + polygon.vertex_count; ++i)
                        {
                            ofbx::Vec2 uv = uvs.get(i);
                            staticMesh.uvs.emplace_back(uv.x, uv.y);
                        }
                    }
                }

                for (int polygon_idx = 0; polygon_idx < partition.polygon_count; ++polygon_idx)
                {
                    const ofbx::GeometryPartition::Polygon& polygon = partition.polygons[polygon_idx];
                    for (int i = polygon.from_vertex; i < polygon.from_vertex + polygon.vertex_count; ++i)
                    {
                        staticMesh.indices.push_back(1 + i + indices_offset);
                    }
                }

                indices_offset += positions.count;
            }
        }

        return ret;
    }
}