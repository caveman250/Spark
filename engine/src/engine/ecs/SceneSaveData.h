#pragma once 

#include "spark.h"

namespace se::ecs 
{
    struct SceneEntityData : reflect::ObjectBase
    {
        SPARK_CLASS()

        SPARK_MEMBER(Serialized)
        Id entity = {};

        SPARK_MEMBER(Serialized)
        std::string name = {};

        SPARK_MEMBER(Serialized)
        int32_t flags = {};

        SPARK_MEMBER(Serialized)
        std::vector<Id> children = {};

        SPARK_MEMBER(Serialized)
        std::vector<ObjectBase*> components = {};
    };

    struct SceneSaveData : reflect::ObjectBase
    {
        SPARK_CLASS()

        SPARK_MEMBER(Serialized)
        std::vector<SceneEntityData> entities = {};
    };
}