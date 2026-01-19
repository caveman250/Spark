#pragma once

#include "spark.h"
#include "engine/ecs/SystemUpdateData.h"
#include "engine/ecs/UpdateMode.h"
#include "engine/threads/ParallelForEach.h"

namespace se::ecs
{
    class SystemUpdateData;
}

namespace se::ecs::util
{
    template <typename Func>
    void ParallelForEachEntity(const SystemUpdateData& data, Func&& func)
    {
        threads::ParallelForEach(
                    std::ranges::iota_view(static_cast<size_t>(0), data.GetEntities().size()),
                    std::forward<Func>(func));
    }
}
