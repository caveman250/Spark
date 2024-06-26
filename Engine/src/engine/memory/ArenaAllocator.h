#pragma once
#include "Arena.h"

namespace se::memory
{
    template <typename T>
    class ArenaAllocator
    {
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

        Arena* m_Arena = nullptr;

        template <class U>
        struct rebind
        {
            typedef ArenaAllocator<U> other;
        };

        explicit ArenaAllocator(Arena& arena)
        {
            m_Arena = &arena;
        }

        ~ArenaAllocator()
        {
        }

        template <typename U>
        ArenaAllocator(const ArenaAllocator<U>& rhs)
        {
            m_Arena = rhs.m_Arena;
        }

        pointer address(reference r)
        {
            return &r;
        }

        const_pointer address(const_reference r)
        {
            return &r;
        }

        pointer allocate(size_type cnt)
        {
            return m_Arena->Alloc<T>(cnt);
        }

        void construct(pointer p, const T& val)
        {
            new(p) T(val);
        }

        void construct(pointer p)
        {
            new(p) T();
        }

        void destroy(pointer p)
        {
            p->~T();
        }

        void deallocate(pointer, size_type)
        {
            // empty
        }

        bool operator==(ArenaAllocator const& a) { return this == &a; }
        bool operator!=(ArenaAllocator const& a) { return !operator==(a); }
    };
}
