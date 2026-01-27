#include "spark.h"

#include "engine/math/Mat4.h"
#include "engine/ecs/components/MeshComponent.h"
#include "TransformSystem.h"

#include "engine/Application.h"
#include "engine/ecs/components/TransformComponent.h"
#include <easy/profiler.h>

#include "RootTransformSystem.h"
#include "engine/ecs/util/SystemUtil.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ecs::systems
{
    SystemDeclaration TransformSystem::GetSystemDeclaration()
    {
        return SystemDeclaration("TransformSystem")
                .WithComponent<TransformComponent>()
                .WithDependency<RootTransformSystem>();
    }

    void TransformSystem::OnUpdate(const SystemUpdateData& updateData)
    {
        EASY_BLOCK("TransformSystem::OnUpdate");

        auto* transform = updateData.GetComponentArray<TransformComponent>();

        for (size_t i = 0; i < updateData.GetEntities().size(); ++i)
        {
            auto& trans = transform[i];
            trans.transform = Translation(trans.pos);
            trans.transform = trans.transform * AxisAngle(math::Vec3(1.0f, 0.0f, 0.0f), trans.rot.x);
            trans.transform = trans.transform * AxisAngle(math::Vec3(0.0f, 1.0f, 0.0f), trans.rot.y);
            trans.transform = trans.transform * AxisAngle(math::Vec3(0.0f, 0.0f, 1.0f), trans.rot.z);
            trans.transform = trans.transform *  Scale(trans.scale);
        }
    }
}
