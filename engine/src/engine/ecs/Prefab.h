#pragma once 

#include "spark.h"
#include "engine/asset/Asset.h"

namespace se::ecs 
{
    struct Component;

    struct PrefabEntity : reflect::ObjectBase
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

    class Prefab : public asset::Asset
    {
        SPARK_CLASS()

        SPARK_MEMBER(Serialized)
        std::vector<PrefabEntity> m_Entities = {};
    };
}