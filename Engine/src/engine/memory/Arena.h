#pragma once
#include "spark.h"

namespace se::memory
{
    struct ObjectRecord
    {
        void* objects_begin;
        uint32_t objects_count;
        ObjectRecord* next;
        virtual void Release(void* obj) = 0;
    };


    template <typename T>
    struct ObjectRecordTemplated : ObjectRecord
    {
        void Release(void* obj) override
        {
            for (uint32_t i = 0; i < objects_count; ++i)
            {
                (static_cast<T*>(obj) + i)->~T();
            }
        }
    };

    class Arena
    {
    public:
        Arena(size_t capacity = 65535);
        ~Arena();
        template <typename T, typename... Args>
        T* Alloc(Args &&... args);
        template <typename T>
        T* Alloc(size_t n);
        void Reset();
        void ResetAndReleaseMemory();

    private:
        void ReleaseObjects() const;

        char* m_Arena;
        ObjectRecord* m_ObjectRecordsBegin = nullptr;
        size_t m_Current = 0; // in chars
        size_t m_Capacity = 0; // in chars
    };

    template <typename T, typename ... Args>
    T* Arena::Alloc(Args &&... args)
    {
        if (!SPARK_VERIFY(m_Current + sizeof(T) < m_Capacity))
        {
            return nullptr;
        }

        char* item = m_Arena + m_Current;
        m_Current += sizeof(T);

        char* recordMemory = m_Arena + m_Current;
        ObjectRecordTemplated<T>* record = new (recordMemory) ObjectRecordTemplated<T>();
        m_Current += sizeof(ObjectRecord);
        record->next = m_ObjectRecordsBegin;
        record->objects_begin = item;
        record->objects_count = 1;
        m_ObjectRecordsBegin = record;

        return new (item) T(std::forward<Args>(args)...);
    }

    template <typename T>
    T* Arena:: Alloc(size_t n)
    {
        if (!SPARK_VERIFY(m_Current + (n * sizeof(T)) <= m_Capacity))
        {
            return nullptr;
        }

        char* item = m_Arena + m_Current;
        m_Current += n * sizeof(T);

        char* recordMemory = m_Arena + m_Current;
        ObjectRecordTemplated<T>* record = new (recordMemory) ObjectRecordTemplated<T>();
        m_Current += sizeof(ObjectRecord);
        record->next = m_ObjectRecordsBegin;
        record->objects_begin = item;
        record->objects_count = static_cast<uint32_t>(n);
        m_ObjectRecordsBegin = record;

        return reinterpret_cast<T*>(item);
    }
}
