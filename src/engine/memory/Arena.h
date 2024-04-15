#pragma once
#include "spark.h"

namespace se::memory
{
    struct ObjectRecord
    {
        void* objects_begin;
        uint32_t objects_count;
        ObjectRecord* next;
        virtual void Release(void* obj, int i) = 0;
    };


    template <typename T>
    struct ObjectRecordTemplated : public ObjectRecord
    {
        void Release(void* obj, int i) override
        {
            ((T*)obj + i)->~T();
        }
    };

    template <typename T>
    class Arena
    {
    public:
        Arena(size_t capacity);
        ~Arena();
        template <typename... Args>
        T* Alloc(Args &&... args);
        T* Alloc(size_t n);
        void Reset();
        void ResetAndReleaseMemory();
        void Print();

        T* m_Arena;

    private:
        void ReleaseObjects();

        ObjectRecord* m_ObjectRecordsBegin = nullptr;
        size_t m_Current = 0; // in chars
        size_t m_Capacity = 0; // in chars

    };

    template <typename T>
    Arena<T>::Arena(size_t capacity)
    {
        m_Capacity = capacity;
        m_Arena = static_cast<T*>(malloc(sizeof(char) * m_Capacity));
    }

    template <typename T>
    Arena<T>::~Arena()
    {
        ResetAndReleaseMemory();
    }

    template <typename T>
    template <typename ... Args>
    T* Arena<T>::Alloc(Args &&... args)
    {
        if (!SPARK_VERIFY(m_Current < m_Capacity))
        {
            return nullptr;
        }
        char* item = (char*)m_Arena + m_Current;
        m_Current += sizeof(T);

        char* recordMemory = (char*)m_Arena + m_Current;
        ObjectRecordTemplated<T>* record = new (recordMemory) ObjectRecordTemplated<T>();
        m_Current += sizeof(ObjectRecord);
        record->next = m_ObjectRecordsBegin;
        record->objects_begin = item;
        record->objects_count = 1;
        m_ObjectRecordsBegin = record;

        return new (item) T(std::forward<Args>(args)...);
    }

    template <typename T>
    T* Arena<T>:: Alloc(size_t n)
    {
        if (!SPARK_VERIFY(m_Current + n - 1 < m_Capacity))
        {
            return nullptr;
        }

        char* item = (char*)m_Arena + m_Current;
        m_Current+= n * sizeof(T);

        char* recordMemory = (char*)m_Arena + m_Current;
        ObjectRecordTemplated<T>* record = new (recordMemory) ObjectRecordTemplated<T>();
        m_Current += sizeof(ObjectRecord);
        record->next = m_ObjectRecordsBegin;
        record->objects_begin = item;
        record->objects_count = n;
        m_ObjectRecordsBegin = record;

        return reinterpret_cast<T*>(item);
    }

    template <typename T>
    void Arena<T>::Reset()
    {
        ReleaseObjects();
        m_Current = 0;
    }

    template <typename T>
    void Arena<T>::ResetAndReleaseMemory()
    {
        ReleaseObjects();
        free(m_Arena);
        m_Current = 0;
    }

    template <typename T>
    void Arena<T>::ReleaseObjects()
    {
        if (m_ObjectRecordsBegin)
        {
            for (ObjectRecord* objRecord = m_ObjectRecordsBegin; objRecord != nullptr; objRecord = objRecord->next)
            {
                void* object = objRecord->objects_begin;
                for (int i = 0; i < objRecord->objects_count; ++i)
                {
                    objRecord->Release(objRecord->objects_begin, i);
                }
            }
        }
    }
}
