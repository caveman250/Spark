module;

#include "spark.h"
#include "engine/ecs/Component.h"

export module Spark.ECS.Components.TransformComponent;
import Spark.Math;
import Spark.Geo.AABB;

namespace se::ecs::components
{
    export struct TransformComponent : Component
    {
        SPARK_COMPONENT()

        SPARK_MEMBER(Serialized)
        math::Vec3 pos = {};

        SPARK_MEMBER(Serialized)
        math::Vec3 rot = {};

        SPARK_MEMBER(Serialized)
        math::Vec3 scale = math::Vec3(1.f, 1.f, 1.f);

        SPARK_MEMBER()
        math::Mat4 transform = math::Translation(math::Vec3::Max());

        SPARK_MEMBER()
        math::Mat4 worldTransform = math::Translation(math::Vec3::Max());

        SPARK_MEMBER()
        geo::AABB aabb = {};

        SPARK_MEMBER()
        const TransformComponent* parent = nullptr;

    private:
    };
}
