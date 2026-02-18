#pragma once

namespace se::threads
{
    struct SpinLock
    {
        void Lock() const noexcept;
        void Unlock() const noexcept;

    private:
        mutable std::atomic<bool> lock = { false };
    };

}
