#pragma once

namespace se::util
{
    template <typename T>
    struct PtrRange
    {
        explicit PtrRange(T* _p, size_t _size)
            : p(_p)
            , size(_size)
        {
        }

        T* p;
        T* begin() const { return p; }
        T* end() const { return p ? p + size : nullptr; }

        size_t size;
    };
}
