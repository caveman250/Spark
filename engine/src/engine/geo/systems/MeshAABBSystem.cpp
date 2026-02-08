#include "MeshAABBSystem.h"

#include "engine/ecs/components/MeshComponent.h"
#include "engine/ecs/components/TransformComponent.h"

namespace se::geo::systems
{
    ecs::SystemDeclaration MeshAABBSystem::GetSystemDeclaration()
    {
        return ecs::SystemDeclaration()
            .WithComponent<const ecs::components::MeshComponent>()
            .WithComponent<ecs::components::TransformComponent>();
    }

    void MeshAABBSystem::OnUpdate(const ecs::SystemUpdateData& updateData)
    {
        const auto* meshes = updateData.GetComponentArray<const ecs::components::MeshComponent>();
        auto* transforms = updateData.GetComponentArray<ecs::components::TransformComponent>();

        for (size_t i = 0; i < updateData.GetEntities().size(); ++i)
        {
            const auto& mesh = meshes[i];
            auto& transform = transforms[i];

            // TODO this could be done on import.
            if (!mesh.model.IsSet())
            {
                return;
            }
            if (transform.aabb.size == 0)
            {
                float minX = std::numeric_limits<float>::max(), minY = std::numeric_limits<float>::max(), minZ = std::numeric_limits<float>::max();
                float maxX = std::numeric_limits<float>::min(), maxY = std::numeric_limits<float>::min(), maxZ = std::numeric_limits<float>::min();
                for (const auto& vert : mesh.model.GetAsset()->GetMesh().vertices)
                {
                    if (vert.x < minX)
                    {
                        minX = vert.x;
                    }
                    if (vert.y < minY)
                    {
                        minY = vert.y;
                    }
                    if (vert.z < minZ)
                    {
                        minZ = vert.z;
                    }
                    if (vert.x > maxX)
                    {
                        maxX = vert.x;
                    }
                    if (vert.y > maxY)
                    {
                        maxY = vert.y;
                    }
                    if (vert.z > maxZ)
                    {
                        maxZ = vert.z;
                    }
                }

                transform.aabb = { {minX, minY, minZ}, { maxX - minX, maxY - minY, maxZ - minZ }};
            }
        }
    }
}
