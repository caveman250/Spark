#pragma once
#include "spark.h"

#if SPARK_PLATFORM_MAC
#include <dispatch/dispatch.h>
#endif

namespace se::threads
{
    template <typename Container, typename Func>
    static void ParallelForEach(Container&& items, Func&& func)
    {
#if SPARK_PLATFORM_MAC
        dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0);
        dispatch_apply(items.size(), queue, ^(size_t i) {
            auto it = items.begin();
            std::advance(it, i);
            func(*it);
        });
#else
        std::for_each(std::execution::par_unseq,
                              items.begin(),
                              items.end(),
                              func);
#endif
    }
}
