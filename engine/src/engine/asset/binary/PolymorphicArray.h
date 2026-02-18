#pragma once

#include "Array.h"
#include "spark.h"

namespace se::asset::binary
{
    class PolymorphicArray 
    {
    public:
        uint32_t GetCount() const;
        std::optional<Struct> GetObjectStruct(size_t i) const;
        std::optional<uint32_t> GetObjectStructIndex(size_t i) const;
        Object Get(size_t i) const;
        void Set(size_t i, const Object& obj) const;
    private:
        PolymorphicArray(uint32_t offset, Database* database);
        std::optional<Object> GetObject(size_t i) const;
        Array GetArray() const;
        bool IsObjectValid(size_t i) const;

        uint32_t m_ArrayOffset;
        Database* m_DB;

        friend class Database;
        friend class Object;
    };
}
