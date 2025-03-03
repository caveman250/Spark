#pragma once
#include "spark.h"

#if SPARK_PLATFORM_MAC
#include <dispatch/dispatch.h>
#endif

namespace se::threads
{
    template <typename T>
    static void ParallelForEach(const std::vector<T>& items, const std::function<void(const T&)> func)
    {
#if SPARK_PLATFORM_MAC
        dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
        dispatch_apply(items.size(), queue, ^(size_t i) {
            func(items[i]);
        });
#else
        std::for_each(std::execution::par_unseq
                              items.begin(),
                              items.end(),
                              func);
#endif
    }
}
