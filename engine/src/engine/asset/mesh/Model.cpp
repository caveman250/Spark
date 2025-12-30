#include "spark.h"
#include "Model.h"
#include "ofbx.h"

namespace se::asset
{
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
                SPARK_ASSERT(partition_idx == 0); // TODO multiple meshes
                StaticMesh& staticMesh = ret->m_Mesh;
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
                        staticMesh.indices.push_back(i + indices_offset);
                    }
                }

                indices_offset += positions.count;
            }
        }

        return ret;
    }
}