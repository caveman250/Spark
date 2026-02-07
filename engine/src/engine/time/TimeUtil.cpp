#include "TimeUtil.h"

namespace se::time
{
    std::time_t Now()
    {
        const auto time = std::chrono::system_clock::now();
        const auto since_epoch = time.time_since_epoch();
        const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch);
        return millis.count();
    }
}
