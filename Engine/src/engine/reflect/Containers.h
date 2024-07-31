#pragma once

#include "Object.h"
#include "spark.h"
#include "TypeResolver.h"
#include "engine/asset/binary/Object.h"

namespace se::reflect
{
    template<typename T>
    struct Type_StdSharedPtr : Type
    {
        Type_StdSharedPtr(T*)
            : Type{"std::shared_ptr<>", sizeof(std::shared_ptr<T>), asset::binary::Type::Bool /* GetBinaryType will reirect to contained type */ }
        {
        }

        std::string GetTypeName(const void*) const override;
        asset::binary::Type GetBinaryType() const override { return TypeResolver<T>::get()->GetBinaryType(); }

        static Type* GetSerialisedType(asset::binary::Object& parentObj, const std::string& fieldName);
        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        asset::binary::StructLayout GetStructLayout(const void*) const override;
        bool IsPolymorphic() const override { return true; }
    };

    template <typename T>
    std::string Type_StdSharedPtr<T>::GetTypeName(const void* obj) const
    {
        if (obj)
        {
            static_assert(!T::s_IsPOD, "Pointer (Polymorphic) serialisation not supported for POD types.");
            auto* typed = static_cast<const std::shared_ptr<T>*>(obj);
            auto* objBase = static_cast<ObjectBase*>(typed->get());
            if (SPARK_VERIFY(objBase))
            {
                return objBase->GetTypeName();
            }

            return "";
        }

        return std::string("std::shared_ptr<") + TypeResolver<T>::get()->GetTypeName(nullptr) + ">";
    }

    Type* TypeFromString(const std::string& type);
    template <typename T>
    Type* Type_StdSharedPtr<T>::GetSerialisedType(asset::binary::Object& parentObj, const std::string& fieldName)
    {
        asset::binary::Object thisObj = fieldName.empty() ? parentObj : parentObj.Get<asset::binary::Object>(fieldName);
        auto typeName = thisObj.GetStruct().GetName();
        return TypeFromString(typeName);
    }

    template <typename T>
    void Type_StdSharedPtr<T>::Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        static_assert(!T::s_IsPOD, "Pointer (Polymorphic) serialisation not supported for POD types.");
        auto* typed = static_cast<const std::shared_ptr<T>*>(obj);
        auto* objBase = static_cast<ObjectBase*>(typed->get());
        if (SPARK_VERIFY(objBase))
        {
            if (!fieldName.empty()) // parent is an object
            {
                asset::binary::StructLayout structLayout = GetStructLayout(obj);
                auto db = parentObj.GetDatabase();
                auto structIndex = db->GetOrCreateStruct(objBase->GetTypeName(), structLayout);
                auto binaryObj = db->CreateObject(structIndex);
                parentObj.Set(fieldName, binaryObj);
                objBase->Serialize(typed->get(), binaryObj, {});
            }
            else // parent is this
            {
                objBase->Serialize(typed->get(), parentObj, {});
            }
        }
    }

    template <typename T>
    void Type_StdSharedPtr<T>::Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        static_assert(!T::s_IsPOD, "Pointer (Polymorphic) serialisation not supported for POD types.");
        auto* typed = static_cast<std::shared_ptr<T>*>(obj);
        auto* objBase = static_cast<ObjectBase*>(typed->get());
        if (SPARK_VERIFY(objBase))
        {
            objBase->Deserialize(typed->get(), parentObj, fieldName);
        }
    }

    template <typename T>
    asset::binary::StructLayout Type_StdSharedPtr<T>::GetStructLayout(const void* obj) const
    {
        static_assert(!T::s_IsPOD, "Pointer (Polymorphic) serialisation not supported for POD types.");
        auto* typed = static_cast<const std::shared_ptr<T>*>(obj);
        auto* objBase = static_cast<ObjectBase*>(typed->get());
        if (SPARK_VERIFY(objBase))
        {
            return objBase->GetStructLayout(obj);
        }

        return {};
    }

    template <typename T>
    class TypeResolver<std::shared_ptr<T>>
    {
    public:
        static Type* get()
        {
            static Type_StdSharedPtr<T> typeDesc{(T*)nullptr};
            return &typeDesc;
        }
    };

    template <typename T> struct is_shared_ptr : std::false_type {};
    template <typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};
    template <typename T> concept SharedPtr = is_shared_ptr<T>::value;
    template <typename T> concept NotSharedPtr = !is_shared_ptr<T>::value;

    template<SharedPtr T>
    T Instantiate(const std::string& type)
    {
        const Type* reflect = reflect::TypeFromString(type);
        T obj = std::shared_ptr<typename T::element_type>(static_cast<typename T::element_type*>(reflect->heap_constructor()));
        return obj;
    }

    template<NotSharedPtr T>
    T Instantiate(const std::string&)
    {
        T obj = {};
        return obj;
    }

    void SerializeArray(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName, Type* itemType,
                    size_t (*getSize)(const void*), const void* (*getItem)(const void*, size_t));
    void DeserializeArray(void* obj, asset::binary::Object& parentObj, const std::string& fieldName, Type* itemType,
                            std::function<void*(void*, size_t)> getRawItem);
    void DeserializeVector(void* obj, asset::binary::Object& parentObj, const std::string& fieldName, Type* itemType,
                        std::function<void*(const void* vecPtr, const std::string&)> emplace_back);

    template <typename T>
    struct Type_StdVector : Type
    {
        Type* itemType;

        size_t (*getSize)(const void*);
        const void* (*getItem)(const void*, size_t);
        std::function<void*(const void* vecPtr, const std::string&)> emplace_back;

        Type_StdVector(T*)
            : Type{"std::vector<>", sizeof(std::vector<T>), asset::binary::Type::Array},
              itemType{TypeResolver<T>::get()}
        {
            getSize = [](const void* vecPtr)
            {
                const auto& vec = *static_cast<const std::vector<T>*>(vecPtr);
                return vec.size();
            };

            getItem = [](const void* vecPtr, size_t index)
            {
                const auto& vec = *static_cast<const std::vector<T>*>(vecPtr);
                return static_cast<const void*>(&vec.at(index));
            };

            emplace_back = [](const void* vecPtr, const std::string& type) -> void* {
                auto& vec = *(std::vector<T>*)vecPtr;
                return &vec.emplace_back(Instantiate<T>(type));
            };
        }

        std::string GetTypeName(const void*) const override;
        asset::binary::Type GetBinaryType() const override { return itemType->IsPolymorphic() ? asset::binary::Type::PolymorphicArray : asset::binary::Type::Array ; }

        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        asset::binary::StructLayout GetStructLayout(const void*) const override;
    };
    template <typename T>
    std::string Type_StdVector<T>::GetTypeName(const void* obj) const
    {
        return std::string("std::vector<") + itemType->GetTypeName(obj) + ">";
    }

    template <typename T>
    void Type_StdVector<T>::Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        SerializeArray(obj, parentObj, fieldName, itemType, getSize, getItem);
    }

    template <typename T>
    void Type_StdVector<T>::Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        DeserializeVector(obj, parentObj, fieldName, itemType, emplace_back);
    }

    template <typename T>
    asset::binary::StructLayout Type_StdVector<T>::GetStructLayout(const void*) const
    {
        asset::binary::StructLayout structLayout = {
            {asset::binary::CreateFixedString32("val"), GetBinaryType()}
        };
        return structLayout;
    }

    template <typename T>
    class TypeResolver<std::vector<T>>
    {
    public:
        static Type* get()
        {
            static Type_StdVector<T> typeDesc{(T*)nullptr};
            return &typeDesc;
        }
    };

    template <size_t Size>
    struct Type_StdArray : Type
    {
        Type* itemType;
        size_t (*getSize)(const void*);
        const void* (*getItem)(const void*, size_t);
        std::function<void*(void*, size_t)> getRawItem;

        template <typename T>
        Type_StdArray(T*)
            : Type{"std::array<>", sizeof(std::array<T, Size>), asset::binary::Type::Array},
              itemType{TypeResolver<T>::get()}
        {
            getSize = [](const void* arrayPtr)
            {
                const auto& vec = *static_cast<const std::array<T, Size>*>(arrayPtr);
                return vec.size();
            };
            getItem = [](const void* arrayPtr, size_t index)
            {
                const auto& vec = *static_cast<const std::array<T, Size>*>(arrayPtr);
                return static_cast<const void*>(&vec.at(index));
            };

            getRawItem = [](void* vecPtr, size_t index) -> void* {
                auto& vec = *(std::array<T, Size>*)vecPtr;
                return &vec[index];
            };
        }

        virtual std::string GetTypeName(const void* obj) const override
        {
            return std::string("std::array<") + itemType->GetTypeName(obj) + ">";
        }

        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override
        {
            SerializeArray(obj, parentObj, fieldName, itemType, getSize, getItem);
        }

        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override
        {
            DeserializeArray(obj, parentObj, fieldName, itemType, getRawItem);
        }

        asset::binary::Type GetBinaryType() const override { return itemType->IsPolymorphic() ? asset::binary::Type::PolymorphicArray : asset::binary::Type::Array; }

        asset::binary::StructLayout GetStructLayout(const void*) const override
        {
            asset::binary::StructLayout structLayout = {
                {asset::binary::CreateFixedString32("val"), GetBinaryType() }
            };
            return structLayout;
        }
    };


    template <typename T, size_t Size>
    class TypeResolver<std::array<T, Size>>
    {
    public:
        static Type* get()
        {
            static Type_StdArray<Size> typeDesc{(T*)nullptr};
            return &typeDesc;
        }
    };

    template <typename T, typename Y>
    struct Type_StdMap : Type
    {
        Type* keyType;
        Type* itemType;

        size_t (*getSize)(const void*);
        std::function<void*(void*, const T& key, const Y& val)> insert;

        Type_StdMap(T*, Y*)
            : Type{"std::map<>", sizeof(std::map<T, Y>), asset::binary::Type::Array}
            , keyType(TypeResolver<T>::get())
            , itemType(TypeResolver<Y>::get())
        {
            getSize = [](const void* vecPtr)
            {
                const auto& vec = *static_cast<const std::map<T, Y>*>(vecPtr);
                return vec.size();
            };
        }

        std::string GetTypeName(const void*) const override;

        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        asset::binary::StructLayout GetStructLayout(const void*) const override;
    };

    template <typename T, typename Y>
    std::string Type_StdMap<T, Y>::GetTypeName(const void* obj) const
    {
        return std::string("std::map<") + keyType->GetTypeName(obj) + ", " + itemType->GetTypeName(obj) + ">";
    }

    template <typename T, typename Y>
    void Type_StdMap<T, Y>::Serialize(const void* obj, asset::binary::Object& parentObj,
        const std::string& fieldName) const
    {
        size_t numItems = getSize(obj);
        auto db = parentObj.GetDatabase();

        asset::binary::StructLayout structLayout = {
            {asset::binary::CreateFixedString32("key"), keyType->GetBinaryType() },
            {asset::binary::CreateFixedString32("value"), itemType->GetBinaryType() },
            {asset::binary::CreateFixedString32("key_type"), asset::binary::Type::String },
            {asset::binary::CreateFixedString32("value_type"), asset::binary::Type::String }
        };
        std::map<T, Y>* map = (std::map<T, Y>*)obj;
        auto array = db->CreateArray(db->GetOrCreateStruct(
                                        std::format("Internal_MapItem{}_{}", TypeToString(keyType->GetBinaryType()), TypeToString(itemType->GetBinaryType())),
                                        structLayout),
                                    numItems);
        int index = 0;
        for (const auto& [key, value] : *map)
        {
            asset::binary::Object arrayObj = array.Get(index);

            arrayObj.Set("key_type", keyType->GetTypeName(&key));
            arrayObj.Set("value_type", itemType->GetTypeName(&value));

            keyType->Serialize(&key, arrayObj, "key");
            itemType->Serialize(&value, arrayObj, "value");
            index++;
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

    template <typename T, typename Y>
    void Type_StdMap<T, Y>::Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        std::map<T, Y>* map = (std::map<T, Y>*)obj;
        auto array = parentObj.Get<asset::binary::Array>(fieldName.empty() ? "val" : fieldName);
        if (array.GetCount() == 0)
        {
            return;
        }

        for (size_t i = 0; i < array.GetCount(); ++i)
        {
            asset::binary::Object arrayObj = array.Get(i);
            T key = Instantiate<T>(arrayObj.GetString("key_type"));
            keyType->Deserialize(&key, arrayObj, "key");
            Y item = Instantiate<Y>(arrayObj.GetString("value_type"));
            itemType->Deserialize(&item, arrayObj, "value");
            map->insert(std::make_pair(key, item));
        }
    }

    template <typename T, typename Y>
    asset::binary::StructLayout Type_StdMap<T, Y>::GetStructLayout(const void*) const
    {
        asset::binary::StructLayout structLayout = {
            {asset::binary::CreateFixedString32("val"), asset::binary::Type::Array}
        };
        return structLayout;
    }

    template <typename T, typename Y>
    class TypeResolver<std::map<T, Y>>
    {
    public:
        static Type* get()
        {
            static Type_StdMap<T, Y> typeDesc{nullptr, nullptr};
            return &typeDesc;
        }
    };
}
