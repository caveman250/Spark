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
    void ForEachEntity(const System* system, const SystemUpdateData& data, Func&& func)
    {
        switch (system->GetUpdateMode())
        {
            case UpdateMode::SingleThreaded:
                for (size_t i = 0; i < data.GetEntities().size(); ++i)
                {
                    func(i);
                }
                break;
            case UpdateMode::MultiThreaded:
                threads::ParallelForEach(
                    std::ranges::iota_view(static_cast<size_t>(0), data.GetEntities().size()),
                    std::forward<Func>(func));
                break;
        }
    }
}
