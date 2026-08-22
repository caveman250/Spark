module;

#include "spark.h"

export module SceneRecord;
import AssetReference;
import Spark.ECS.Prefab;

namespace se::ecs
{
    export struct PrefabRecord
    {
        asset::AssetReference<Prefab> prefab;
        Id entity;
    };

    export struct SceneRecord
    {
        std::string path = {};
        std::vector<Id> entities = {};
        std::vector<PrefabRecord> prefabs = {};
    };
}
