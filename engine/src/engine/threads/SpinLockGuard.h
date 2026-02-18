#pragma once
#include "SpinLock.h"

namespace se::threads
{
    struct SpinLockGuard
    {
        explicit SpinLockGuard(const SpinLock& _spinLock) : spinLock(_spinLock) { spinLock.Lock(); }
        ~SpinLockGuard() { spinLock.Unlock(); }
    private:
        const SpinLock& spinLock;
    };
}
