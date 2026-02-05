#include "spark.h"
#include "Model.h"
#include "ofbx.h"

namespace se::asset
{
    ofbx::Vec3 ConvertPosition(int upAxis, ofbx::Vec3 v)
    {
        switch (upAxis)
        {
            case 0:
                SPARK_ASSERT(false); // need to test this with a real asset.
                return v;
            case 1:
                break;
            case 2:
            {
                return {
                    v.x,
                    v.z,
                   -v.y
                };
            }
            default:
                SPARK_ASSERT(false);
                return v;
        }
    }

    std::shared_ptr<Model> Model::FromFBX(ofbx::IScene* fbxScene)
    {
        auto ret = std::make_shared<Model>();
        StaticMesh& staticMesh = ret->m_Mesh;

        const ofbx::GlobalSettings* settings = fbxScene->getGlobalSettings();
        int upAxis = settings->UpAxis;        // 0=X, 1=Y, 2=Z
        int upSign = settings->UpAxisSign;    // usually +1
        double unitScale = settings->UnitScaleFactor;

        // i want 1, 1, 1

        int mesh_count = fbxScene->getMeshCount();

        // output unindexed geometry
        for (int mesh_idx = 0; mesh_idx < mesh_count; ++mesh_idx)
        {
            const ofbx::Mesh& mesh = *fbxScene->getMesh(mesh_idx);
            const ofbx::GeometryData& geom = mesh.getGeometryData();

            const ofbx::Vec3Attributes positions = geom.getPositions();
            const ofbx::Vec3Attributes normals = geom.getNormals();
            const ofbx::Vec2Attributes uvs = geom.getUVs();

            //const ofbx::DMatrix transform = mesh.getGlobalTransform();

            for (int partition_idx = 0; partition_idx < geom.getPartitionCount(); ++partition_idx)
            {
                //SPARK_ASSERT(partition_idx == 0); // TODO multiple meshes

                const ofbx::GeometryPartition& partition = geom.getPartition(partition_idx);

                for (int polygon_idx = 0; polygon_idx < partition.polygon_count; ++polygon_idx)
                {
                    const ofbx::GeometryPartition::Polygon& poly = partition.polygons[polygon_idx];

                    for (int i = 1; i + 1 < poly.vertex_count; ++i)
                    {
                        const int idx[3] = {
                            poly.from_vertex,
                            poly.from_vertex + i,
                            poly.from_vertex + i + 1
                        };

                        for (int v = 0; v < 3; ++v)
                        {
                            const int pv = idx[v];

                            ofbx::Vec3 p = ConvertPosition(upAxis, positions.get(pv));
                            //ofbx::DVec3 tp = transform * ofbx::DVec3(p.x, p.y, p.z);
                            //ofbx::DVec3 tp = ofbx::DVec3(p.x, p.y, p.z);
                            staticMesh.vertices.emplace_back(p.x, p.y, p.z);

                            if (normals.values)
                            {
                                ofbx::Vec3 n = ConvertPosition(upAxis, normals.get(pv));
                                staticMesh.normals.emplace_back(n.x, n.y, n.z);
                            }

                            if (uvs.values)
                            {
                                int uv_index = pv;

                                if (uvs.indices)
                                    uv_index = uvs.indices[pv];

                                ofbx::Vec2 uv = uvs.values[uv_index];
                                staticMesh.uvs.emplace_back(uv.x, 1.0f - uv.y);
                            }

                            staticMesh.indices.push_back(
                                static_cast<uint32_t>(staticMesh.vertices.size() - 1));
                        }
                    }
                }
            }
        }

        return ret;
    }
}