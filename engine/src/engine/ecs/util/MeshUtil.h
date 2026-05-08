#pragma once

namespace se::asset
{
    struct StaticMesh;
}

namespace se::ecs::components
{
    struct MeshComponent;
}

namespace se::ecs::util
{
    void InitMeshComponentFromMesh(components::MeshComponent* meshComp, const asset::StaticMesh& mesh);
}
