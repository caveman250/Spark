#include "Containers.h"

#include "engine/asset/binary/Object.h"

namespace se::reflect
{
    std::string Type_StdVector::GetTypeName() const
    {
        return std::string("std::vector<") + itemType->GetTypeName() + ">";
    }

    void Type_StdVector::Serialize(const void* obj, asset::binary::Object& parentObj,
                                   const std::string& fieldName) const
    {
        size_t numItems = getSize(obj);
        auto db = parentObj.GetDatabase();
        auto array = db->CreateArray(db->GetOrCreateStruct(itemType->GetStructLayout()), numItems);
        for (size_t index = 0; index < numItems; index++)
        {
            asset::binary::Object arrayObj = array.Get(index);
            itemType->Serialize(getItem(obj, index), arrayObj, {});
        }

        if (!fieldName.empty())
        {
            parentObj.Set(fieldName, array);
        }
        else
        {
            parentObj.Set("val", array);
        }
    }

    void Type_StdVector::Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        auto array = parentObj.Get<asset::binary::Array>(fieldName.empty() ? "val" : fieldName);
        if (array.GetCount() == 0)
        {
            instantiate(obj, 0);
        }
        else
        {
            instantiate(obj, array.GetCount());
            for (size_t i = 0; i < array.GetCount(); ++i)
            {
                asset::binary::Object arrayObj = array.Get(i);
                itemType->Deserialize(getRawItem(obj, i), arrayObj, {});
            }
        }
    }

    asset::binary::StructLayout Type_StdVector::GetStructLayout() const
    {
        asset::binary::StructLayout structLayout = {
            {asset::binary::CreateFixedString32("val"), asset::binary::Type::Array}
        };
        return structLayout;
    }

    void SerializeArray(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName, Type* itemType,
                        size_t (*getSize)(const void*), const void*(*getItem)(const void*, size_t))
    {
        size_t numItems = getSize(obj);
        auto db = parentObj.GetDatabase();
        auto array = db->CreateArray(db->GetOrCreateStruct(itemType->GetStructLayout()), numItems);
        for (size_t index = 0; index < numItems; index++)
        {
            asset::binary::Object arrayObj = array.Get(index);
            itemType->Serialize(getItem(obj, index), arrayObj, {});
        }

        if (!fieldName.empty())
        {
            parentObj.Set(fieldName, array);
        }
        else
        {
            parentObj.Set("val", array);
        }
    }

    void DeserializeArray(void* obj, asset::binary::Object& parentObj, const std::string& fieldName, Type* itemType,
        std::function<void(void*&)> instantiate, std::function<void*(void*, size_t)> getRawItem)
    {
        auto array = parentObj.Get<asset::binary::Array>(fieldName.empty() ? "val" : fieldName);
        if (array.GetCount() == 0)
        {
            instantiate(obj);
        }
        else
        {
            instantiate(obj);
            for (size_t i = 0; i < array.GetCount(); ++i)
            {
                asset::binary::Object arrayObj = array.Get(i);
                itemType->Deserialize(getRawItem(obj, i), arrayObj, {});
            }
        }
    }
}
