#include "MeshUtil.h"
#include "engine/ecs/components/MeshComponent.h"
#include "engine/render/IndexBuffer.h"

namespace se::ecs::util
{
    void InitMeshComponentFromMesh(components::MeshComponent* meshComp, const asset::StaticMesh& mesh)
    {
        meshComp->vertexBuffer = render::VertexBuffer::CreateVertexBuffer(mesh);
        meshComp->vertexBuffer->CreatePlatformResource();
        meshComp->indexBuffer = render::IndexBuffer::CreateIndexBuffer(mesh);
        meshComp->indexBuffer->CreatePlatformResource();
        meshComp->aabb = mesh.aabb;
    }
}
