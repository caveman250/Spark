#include "spark.h"

#include <engine/math/Mat4.h>
#include <engine/ecs/components/MeshComponent.h>
#include "TransformSystem.h"

#include "engine/Application.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/ecs/relationships/ChildOf.h"

using namespace se;
using namespace se::ecs::components;

namespace se::ecs::systems
{
    DEFINE_SPARK_SYSTEM(TransformSystem)

    void TransformSystem::OnUpdate(const std::vector<Id>& entities, TransformComponent* transform)
    {
        for (size_t i = 0; i < entities.size(); ++i)
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
