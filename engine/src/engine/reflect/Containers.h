#pragma once

#include "Object.h"
#include "spark.h"
#include "TypeResolver.h"
#include "engine/asset/binary/Database.h"
#include "engine/asset/binary/Object.h"

namespace se::reflect
{
    Type* TypeFromString(const std::string& type);

    struct Type_Container : Type
    {
        Type_Container(const std::string& name, size_t size, asset::binary::Type binaryType)
                : Type(name, size, binaryType)
        {}

        virtual std::string GetContainerTypeName() const = 0;
        virtual Type* GetContainedKeyType() const { SPARK_ASSERT(false, "GetContainedKeyType - Not implemented for type."); return nullptr; }
        virtual Type* GetContainedValueType(const void* obj) const = 0;
        bool IsContainer() const override { return true; }
        virtual void* GetContainedValue(void*) const { SPARK_ASSERT(false, "GetContainedValue - Not implemented for type."); return nullptr; }
        virtual const void* GetContainedKeyByIndex(void*, size_t) const { SPARK_ASSERT(false, "GetContainedKeyByIndex - Not implemented for type."); return nullptr; }
        virtual void* GetContainedValueByIndex(void*, size_t) const { SPARK_ASSERT(false, "GetContainedValueByIndex - Not implemented for type."); return nullptr; }
        virtual size_t GetNumContainedElements(void*) const = 0;
    };

    struct Type_StdSharedPtrBase : Type_Container
    {
        Type_StdSharedPtrBase(const std::string& name, size_t size, asset::binary::Type binaryType)
            : Type_Container(name, size, binaryType)
        {}

        bool RequiresExplicitInstantiationWithinClass() const override
        {
            return true;
        }
    };

    template<typename T>
    struct Type_StdSharedPtr : Type_StdSharedPtrBase
    {
        Type_StdSharedPtr(T*)
            : Type_StdSharedPtrBase{"std::shared_ptr<>", sizeof(std::shared_ptr<T>), asset::binary::Type::Bool /* GetBinaryType will reirect to contained type */ }
        {
        }

        std::string GetTypeName(const void*) const override;
        asset::binary::Type GetBinaryType() const override { return TypeResolver<T>::get()->GetBinaryType(); }

        static Type* GetSerialisedType(asset::binary::Object& parentObj, const std::string& fieldName);
        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        asset::binary::StructLayout GetStructLayout(const void*) const override;
        bool IsPolymorphic() const override { return true; }
        bool IsContainer() const override { return true; }
        std::string GetContainerTypeName() const override { return "std::shared_ptr<>"; }
        Type* GetContainedValueType(const void* obj) const override;

        void* GetContainedValue(void* obj) const override
        {
            if (SPARK_VERIFY(obj))
            {
                auto* typed = static_cast<const std::shared_ptr<T>*>(obj);
                return typed->get();
            }

            return nullptr;
        }

        size_t GetNumContainedElements(void*) const override
        {
            return 1;
        }

        void* Instantiate(const std::string& type, void* obj) const override
        {
            const Type* reflect = TypeFromString(type);
            *(std::shared_ptr<T>*)obj = std::shared_ptr<T>(static_cast<T*>(reflect->heap_constructor()));
            return obj;
        }
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

    template<typename T>
    Type* Type_StdSharedPtr<T>::GetContainedValueType(const void* obj) const
    {
        if (obj)
        {
            auto* typed = static_cast<const std::shared_ptr<T>*>(obj);
            auto* objBase = static_cast<ObjectBase*>(typed->get());
            return objBase->GetReflectType();
        }
        return TypeResolver<T>::get();
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
    T InstantiateContainerObj(const std::string& type)
    {
        const Type* reflect = TypeFromString(type);
        T obj = std::shared_ptr<typename T::element_type>(static_cast<typename T::element_type*>(reflect->heap_constructor()));
        return obj;
    }

    template<NotSharedPtr T>
    T InstantiateContainerObj(const std::string&)
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
    struct Type_StdVector : Type_Container
    {
        Type* itemType;

        size_t (*getSize)(const void*);
        const void* (*getItem)(const void*, size_t);
        std::function<void*(const void* vecPtr, const std::string&)> emplace_back;

        Type_StdVector(T*)
            : Type_Container{"std::vector<>", sizeof(std::vector<T>), asset::binary::Type::Array},
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
                return &vec.emplace_back(InstantiateContainerObj<T>(type));
            };
        }

        std::string GetTypeName(const void*) const override;
        asset::binary::Type GetBinaryType() const override { return itemType->IsPolymorphic() ? asset::binary::Type::PolymorphicArray : asset::binary::Type::Array ; }

        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        asset::binary::StructLayout GetStructLayout(const void*) const override;
        bool IsContainer() const override { return "true"; }
        std::string GetContainerTypeName() const override { return "std::vector<>"; }
        Type * GetContainedValueType(const void*) const override { return TypeResolver<T>::get(); }
        void* GetContainedValueByIndex(void* obj, size_t i) const override;
        size_t GetNumContainedElements(void* obj) const override
        {
            if (SPARK_VERIFY(obj))
            {
                auto* typed = static_cast<const std::vector<T>*>(obj);
                return typed->size();
            }

            return 0;
        }
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

    template<typename T>
    void* Type_StdVector<T>::GetContainedValueByIndex(void* obj,
                                                      size_t i) const
    {
        if (SPARK_VERIFY(obj))
        {
            auto* typed = static_cast<std::vector<T>*>(obj);
            if constexpr (std::is_pointer_v<T>)
            {
                return typed->at(i);
            }
            else
            {
                return &typed->at(i);
            }
        }

        return nullptr;
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

    template <typename T, size_t Size>
    struct Type_StdArray : Type_Container
    {
        Type* itemType;
        size_t (*getSize)(const void*);
        const void* (*getItem)(const void*, size_t);
        std::function<void*(void*, size_t)> getRawItem;

        Type_StdArray(T*)
            : Type_Container{"std::array<>", sizeof(std::array<T, Size>), asset::binary::Type::Array},
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
            return std::format("std::array<{}, {}>", itemType->GetTypeName(obj), size);
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

        bool IsContainer() const override { return "true"; }
        std::string GetContainerTypeName() const override { return "std::array<>"; }
        Type * GetContainedValueType(const void*) const override { return itemType; }
        void* GetContainedValueByIndex(void* obj, size_t i) const override;
        size_t GetNumContainedElements(void*) const override
        {
            return Size;
        }
    };

    template<typename T, size_t Size>
    void* Type_StdArray<T, Size>::GetContainedValueByIndex(void* obj,
                                                           size_t i) const
    {
        if (SPARK_VERIFY(obj))
        {
            auto* typed = static_cast<std::array<T, Size>*>(obj);
            if constexpr (std::is_pointer_v<T>)
            {
                return typed->at(i);
            }
            else
            {
                return &typed->at(i);
            }
        }

        return nullptr;
    }

    template <typename T, size_t Size>
    class TypeResolver<std::array<T, Size>>
    {
    public:
        static Type* get()
        {
            static Type_StdArray<T, Size> typeDesc{(T*)nullptr};
            return &typeDesc;
        }
    };

    template <typename T, typename Y>
    struct Type_StdMap : Type_Container
    {
        Type* keyType;
        Type* itemType;

        size_t (*getSize)(const void*);
        std::function<void*(void*, const T& key, const Y& val)> insert;

        Type_StdMap(T*, Y*)
            : Type_Container{"std::map<>", sizeof(std::map<T, Y>), asset::binary::Type::Array}
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
        bool IsContainer() const override { return "true"; }
        std::string GetContainerTypeName() const override { return "std::map<>"; }
        Type * GetContainedKeyType() const override { return TypeResolver<T>::get(); }
        Type* GetContainedValueType(const void*) const override { return TypeResolver<Y>::get(); }
        void * GetContainedValueByIndex(void*, size_t i) const override;
        const void * GetContainedKeyByIndex(void*, size_t) const override;
        size_t GetNumContainedElements(void* obj) const override
        {
            if (SPARK_VERIFY(obj))
            {
                auto* typed = static_cast<const std::map<T, Y>*>(obj);
                return typed->size();
            }

            return 0;
        }
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
            {asset::binary::CreateFixedString32("value"), itemType->GetBinaryType() }
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
            T key = InstantiateContainerObj<T>(arrayObj.GetNativeTypeString("key"));
            keyType->Deserialize(&key, arrayObj, "key");
            Y item = InstantiateContainerObj<Y>(arrayObj.GetNativeTypeString("value"));
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

    template<typename T, typename Y>
    void* Type_StdMap<T, Y>::GetContainedValueByIndex(void* obj,
                                                           size_t i) const
    {
        if (SPARK_VERIFY(obj))
        {
            auto* typed = static_cast<std::map<T, Y>*>(obj);
            auto it = typed->begin();
            std::advance(it, i);
            if constexpr (std::is_pointer_v<Y>)
            {
                return it->second;
            }
            else
            {
                return &it->second;
            }
        }

        return nullptr;
    }

    template<typename T, typename Y>
    const void* Type_StdMap<T, Y>::GetContainedKeyByIndex(void* obj,
                                                      size_t i) const
    {
        if (SPARK_VERIFY(obj))
        {
            auto* typed = static_cast<std::map<T, Y>*>(obj);
            auto it = typed->begin();
            std::advance(it, i);
            if constexpr (std::is_pointer_v<T>)
            {
                return it->first;
            }
            else
            {
                return &it->first;
            }
        }

        return nullptr;
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

    template <typename T, typename Y>
    struct Type_StdUnorderedMap : Type_Container
    {
        Type* keyType;
        Type* itemType;

        size_t (*getSize)(const void*);
        std::function<void*(void*, const T& key, const Y& val)> insert;

        Type_StdUnorderedMap(T*, Y*)
            : Type_Container{"std::unordered_map<>", sizeof(std::unordered_map<T, Y>), asset::binary::Type::Array}
            , keyType(TypeResolver<T>::get())
            , itemType(TypeResolver<Y>::get())
        {
            getSize = [](const void* vecPtr)
            {
                const auto& vec = *static_cast<const std::unordered_map<T, Y>*>(vecPtr);
                return vec.size();
            };
        }

        std::string GetTypeName(const void*) const override;

        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        asset::binary::StructLayout GetStructLayout(const void*) const override;
        bool IsContainer() const override { return "true"; }
        std::string GetContainerTypeName() const override { return "std::unordered_map<>"; }
        Type * GetContainedKeyType() const override { return TypeResolver<T>::get(); }
        Type* GetContainedValueType(const void*) const override { return TypeResolver<Y>::get(); }
        void* GetContainedValueByIndex(void *, size_t i) const override;
        const void * GetContainedKeyByIndex(void *, size_t) const override;
        size_t GetNumContainedElements(void* obj) const override
        {
            if (SPARK_VERIFY(obj))
            {
                auto* typed = static_cast<const std::unordered_map<T, Y>*>(obj);
                return typed->size();
            }

            return 0;
        }
    };

    template <typename T, typename Y>
    std::string Type_StdUnorderedMap<T, Y>::GetTypeName(const void* obj) const
    {
        return std::string("std::unordered_map<") + keyType->GetTypeName(obj) + ", " + itemType->GetTypeName(obj) + ">";
    }

    template <typename T, typename Y>
    void Type_StdUnorderedMap<T, Y>::Serialize(const void* obj, asset::binary::Object& parentObj,
        const std::string& fieldName) const
    {
        size_t numItems = getSize(obj);
        auto db = parentObj.GetDatabase();

        asset::binary::StructLayout structLayout = {
            {asset::binary::CreateFixedString32("key"), keyType->GetBinaryType() },
            {asset::binary::CreateFixedString32("value"), itemType->GetBinaryType() }
        };
        std::unordered_map<T, Y>* map = (std::unordered_map<T, Y>*)obj;
        auto array = db->CreateArray(db->GetOrCreateStruct(
                                        std::format("Internal_MapItem{}_{}", TypeToString(keyType->GetBinaryType()), TypeToString(itemType->GetBinaryType())),
                                        structLayout),
                                    numItems);
        int index = 0;
        for (const auto& [key, value] : *map)
        {
            asset::binary::Object arrayObj = array.Get(index);
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
    void Type_StdUnorderedMap<T, Y>::Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        std::unordered_map<T, Y>* map = (std::unordered_map<T, Y>*)obj;
        auto array = parentObj.Get<asset::binary::Array>(fieldName.empty() ? "val" : fieldName);
        if (array.GetCount() == 0)
        {
            return;
        }

        for (size_t i = 0; i < array.GetCount(); ++i)
        {
            asset::binary::Object arrayObj = array.Get(i);
            T key = InstantiateContainerObj<T>(arrayObj.GetNativeTypeString("key"));
            keyType->Deserialize(&key, arrayObj, "key");
            Y item = InstantiateContainerObj<Y>(arrayObj.GetNativeTypeString("value"));
            itemType->Deserialize(&item, arrayObj, "value");
            map->insert(std::make_pair(key, item));
        }
    }

    template <typename T, typename Y>
    asset::binary::StructLayout Type_StdUnorderedMap<T, Y>::GetStructLayout(const void*) const
    {
        asset::binary::StructLayout structLayout = {
            {asset::binary::CreateFixedString32("val"), asset::binary::Type::Array}
        };
        return structLayout;
    }

    template<typename T, typename Y>
    void* Type_StdUnorderedMap<T, Y>::GetContainedValueByIndex(void* obj,
                                                      size_t i) const
    {
        if (SPARK_VERIFY(obj))
        {
            auto* typed = static_cast<std::unordered_map<T, Y>*>(obj);
            auto it = typed->begin();
            std::advance(it, i);
            if constexpr (std::is_pointer_v<Y>)
            {
                return it->second;
            }
            else
            {
                return &(it->second);
            }
        }

        return nullptr;
    }

    template<typename T, typename Y>
    const void* Type_StdUnorderedMap<T, Y>::GetContainedKeyByIndex(void* obj,
                                                    size_t i) const
    {
        if (SPARK_VERIFY(obj))
        {
            auto* typed = static_cast<std::map<T, Y>*>(obj);
            auto it = typed->begin();
            std::advance(it, i);
            if constexpr (std::is_pointer_v<T>)
            {
                return it->first;
            }
            else
            {
                return &it->first;
            }
        }

        return nullptr;
    }

    template <typename T, typename Y>
    class TypeResolver<std::unordered_map<T, Y>>
    {
    public:
        static Type* get()
        {
            static Type_StdUnorderedMap<T, Y> typeDesc{nullptr, nullptr};
            return &typeDesc;
        }
    };
}
