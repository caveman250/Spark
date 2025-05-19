#include "SpinLock.h"

namespace se::threads
{
    void SpinLock::Lock() const noexcept
    {
        for (;;)
        {
            if (!lock.exchange(true, std::memory_order_acquire))
            {
                return;
            }
            while (lock.load(std::memory_order_relaxed))
            {

            }
        }
    }

    void SpinLock::Unlock() const noexcept
    {
        lock.store(false, std::memory_order_release);
    }
}
