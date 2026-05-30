#pragma once

#include "spark.h"
#include "engine/asset/AssetReference.h"

namespace se::ecs
{
    struct PrefabRecord
    {
        asset::AssetReference<Prefab> prefab;
        Id entity;
    };
    struct SceneRecord
    {
        std::string path = {};
        std::vector<Id> entities = {};
        std::vector<PrefabRecord> prefabs = {};
    };
}
