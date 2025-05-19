#include "PolymorphicArray.h"

#include "Object.h"

namespace se::asset::binary
{
    uint32_t PolymorphicArray::GetCount()
    {
        return GetArray().GetCount();
    }

    std::optional<Struct> PolymorphicArray::GetObjectStruct(size_t i)
    {
        auto obj = GetObject(i);
        if (obj.has_value())
        {
            return obj.value().GetStruct();
        }
        else
        {
            return std::nullopt;
        }
    }

    std::optional<uint32_t> PolymorphicArray::GetObjectStructIndex(size_t i)
    {
        auto obj = GetObject(i);
        if (obj.has_value())
        {
            return obj.value().GetStructIndex();
        }
        else
        {
            return std::nullopt;
        }
    }

    Object PolymorphicArray::Get(size_t i)
    {
        SPARK_ASSERT(IsObjectValid(i));
        return GetObject(i).value();
    }

    void PolymorphicArray::Set(size_t i, const Object& obj)
    {
        SPARK_ASSERT(obj.GetDatabase() == m_DB);
        auto recordObj = GetArray().Get(i);
        recordObj.Set("offset", obj.GetOffset());
        recordObj.Set("valid", true);
    }

    PolymorphicArray::PolymorphicArray(uint32_t offset, Database* database)
        : m_ArrayOffset(offset)
        , m_DB(database)
    {
    }

    std::optional<Object> PolymorphicArray::GetObject(size_t i)
    {
        if (SPARK_VERIFY(IsObjectValid(i)))
        {
            auto obj = GetArray().Get(i);
            return m_DB->GetObjectAt(obj.Get<uint32_t>("offset"));
        }

        return std::nullopt;
    }

    Array PolymorphicArray::GetArray() const
    {
        return m_DB->GetArrayAt(m_ArrayOffset);
    }

    bool PolymorphicArray::IsObjectValid(size_t i) const
    {
        auto obj = GetArray().Get(i);
        return obj.Get<bool>("valid");
    }
}
