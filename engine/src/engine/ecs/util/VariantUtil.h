#pragma once

#include "spark.h"

namespace se::ecs::util
{
    template<typename T>
    void CollectVariantComponentIds(std::vector<Id>& components)
    {
        components.emplace_back(T::GetComponentId());
    }
}