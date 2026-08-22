module;

#include "spark.h"

#include "engine/ecs/Component.h"

export module SceneEntityData;

import AssetReference;
import Spark.Asset.Asset;
import Spark.Math;
import Spark.ECS.Prefab;

namespace se::ecs 
{
    export struct SceneEntityData : reflect::ObjectBase
    {
        SPARK_CLASS()

        SPARK_MEMBER(Serialized)
        uint64_t entity = {};

        SPARK_MEMBER(Serialized)
        std::string name = {};

        SPARK_MEMBER(Serialized)
        int32_t flags = {};

        SPARK_MEMBER(Serialized)
        std::vector<uint64_t> children = {};

        SPARK_MEMBER(Serialized)
        std::vector<Component*> components = {};
    };

    export struct ScenePrefabData : reflect::ObjectBase
    {
        SPARK_CLASS()

        SPARK_MEMBER(Serialized)
        asset::AssetReference<Prefab> prefab = {};

        SPARK_MEMBER(Serialized)
        math::Vec3 pos = {};

        SPARK_MEMBER(Serialized)
        math::Vec3 rot = {};

        SPARK_MEMBER(Serialized)
        math::Vec3 scale = {};
    };

    export class SceneSaveData : public asset::Asset
    {
        SPARK_CLASS()

        SPARK_MEMBER(Serialized)
        std::vector<SceneEntityData> m_Entities = {};

        SPARK_MEMBER(Serialized)
        std::vector<ScenePrefabData> m_Prefabs = {};
    };
}
