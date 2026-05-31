#pragma once 

#include "spark.h"
#include "engine/asset/Asset.h"
#include "engine/asset/AssetReference.h"
#include "engine/math/Vec3.h"
#include "engine/ecs/Prefab.h"

namespace se::ecs 
{
    struct Component;

    struct SceneEntityData : reflect::ObjectBase
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

    struct ScenePrefabData : reflect::ObjectBase
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

    class SceneSaveData : public asset::Asset
    {
        SPARK_CLASS()

        SPARK_MEMBER(Serialized)
        std::vector<SceneEntityData> m_Entities = {};

        SPARK_MEMBER(Serialized)
        std::vector<ScenePrefabData> m_Prefabs = {};
    };
}
