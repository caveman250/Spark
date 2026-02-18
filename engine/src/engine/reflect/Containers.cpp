#include "Containers.h"

#include "Reflect.h"
#include "engine/asset/binary/Object.h"

namespace se::reflect
{
    void SerializeArray(const void* obj,
                        asset::binary::Object& parentObj,
                        const std::string& fieldName,
                        const Type* itemType,
                        size_t (*getSize)(const void*), const void*(*getItem)(const void*, size_t))
    {
        const size_t numItems = getSize(obj);
        const auto db = parentObj.GetDatabase();
        if (itemType->IsPolymorphic())
        {
            const auto array = db->CreatePolymorphicArray(numItems);
            for (size_t index = 0; index < numItems; index++)
            {
                const Type_StdSharedPtr<ObjectBase>* reflect = static_cast<Type_StdSharedPtr<ObjectBase>*>(TypeResolver<std::shared_ptr<ObjectBase>>::Get());
                auto* objBase = static_cast<const std::shared_ptr<ObjectBase>*>(getItem(obj, index));
                asset::binary::Object arrayObj = db->CreateObject(
                    db->GetOrCreateStruct(reflect->GetTypeName(objBase), reflect->GetStructLayout(objBase)));
                itemType->Serialize(objBase, arrayObj, {});

                array.Set(index, arrayObj);
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
        else
        {
            auto array = db->CreateArray(db->GetOrCreateStruct(itemType->GetTypeName(nullptr), itemType->GetStructLayout(nullptr)), numItems);
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
    }

    void DeserializeArray(void* obj,
                            const asset::binary::Object& parentObj,
                            const std::string& fieldName,
                            const Type* itemType,
                            const std::function<void*(void*, size_t)>& getRawItem)
    {
        if (itemType->IsPolymorphic())
        {
            const auto array = parentObj.Get<asset::binary::PolymorphicArray>(fieldName.empty() ? "val" : fieldName);
            for (size_t i = 0; i < array.GetCount(); ++i)
            {
                asset::binary::Object arrayObj = array.Get(i);
                itemType->Deserialize(getRawItem(obj, i), arrayObj, {});
            }
        }
        else
        {
            auto array = parentObj.Get<asset::binary::Array>(fieldName.empty() ? "val" : fieldName);
            for (size_t i = 0; i < array.GetCount(); ++i)
            {
                asset::binary::Object arrayObj = array.Get(i);
                itemType->Deserialize(getRawItem(obj, i), arrayObj, {});
            }
        }
    }

    void DeserializeVector(void* obj,
                             const asset::binary::Object& parentObj,
                             const std::string& fieldName,
                             const Type* itemType,
                             const std::function<void*(void* vecPtr, const std::string&)>& emplace_back)
    {
        if (itemType->IsPolymorphic())
        {
            const auto array = parentObj.Get<asset::binary::PolymorphicArray>(fieldName.empty() ? "val" : fieldName);
            for (size_t i = 0; i < array.GetCount(); ++i)
            {
                asset::binary::Object arrayObj = array.Get(i);
                void* newObj = emplace_back(obj, arrayObj.GetStruct().GetName());
                itemType->Deserialize(newObj, arrayObj, {});
            }
        }
        else
        {
            auto array = parentObj.Get<asset::binary::Array>(fieldName.empty() ? "val" : fieldName);
            for (size_t i = 0; i < array.GetCount(); ++i)
            {
                asset::binary::Object arrayObj = array.Get(i);
                void* newObj = emplace_back(obj, "");
                itemType->Deserialize(newObj, arrayObj, {});
            }
        }
    }

    Type* TypeFromString(const std::string& type)
    {
        return TypeLookup::GetType(type);
    }
}
