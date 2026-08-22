module;

#include "spark.h"
#include "easy/profiler.h"



module TransformSystem;
import SystemUtil;
import Application;
import RootTransformSystem;

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

    void TransformSystem::OnUpdate(const QueryResults& results)
    {
        EASY_BLOCK("TransformSystem::OnUpdate");

        ForEachArcheType(results, UpdateMode::MultiThreaded, false, [](const SystemUpdateData& updateData)
        {
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
        });
    }
}
