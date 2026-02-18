#include "PolymorphicArray.h"

#include "Object.h"
#include "Database.h"

namespace se::asset::binary
{
    uint32_t PolymorphicArray::GetCount() const
    {
        return GetArray().GetCount();
    }

    std::optional<Struct> PolymorphicArray::GetObjectStruct(const size_t i) const
    {
        const auto obj = GetObject(i);
        if (obj.has_value())
        {
            return obj.value().GetStruct();
        }
        else
        {
            return std::nullopt;
        }
    }

    std::optional<uint32_t> PolymorphicArray::GetObjectStructIndex(const size_t i) const
    {
        const auto obj = GetObject(i);
        if (obj.has_value())
        {
            return obj.value().GetStructIndex();
        }
        else
        {
            return std::nullopt;
        }
    }

    Object PolymorphicArray::Get(const size_t i) const
    {
        SPARK_ASSERT(IsObjectValid(i));
        return GetObject(i).value();
    }

    void PolymorphicArray::Set(const size_t i, const Object& obj) const
    {
        SPARK_ASSERT(obj.GetDatabase() == m_DB);
        auto recordObj = GetArray().Get(i);
        recordObj.Set("offset", obj.GetOffset());
        recordObj.Set("valid", true);
    }

    PolymorphicArray::PolymorphicArray(const uint32_t offset, Database* database)
        : m_ArrayOffset(offset)
        , m_DB(database)
    {
    }

    std::optional<Object> PolymorphicArray::GetObject(const size_t i) const
    {
        if (SPARK_VERIFY(IsObjectValid(i)))
        {
            const auto obj = GetArray().Get(i);
            return m_DB->GetObjectAt(obj.Get<uint32_t>("offset"));
        }

        return std::nullopt;
    }

    Array PolymorphicArray::GetArray() const
    {
        return m_DB->GetArrayAt(m_ArrayOffset);
    }

    bool PolymorphicArray::IsObjectValid(const size_t i) const
    {
        const auto obj = GetArray().Get(i);
        return obj.Get<bool>("valid");
    }
}
