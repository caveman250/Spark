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
        Type_Container(const std::string& name,
                         const size_t size,
                         const asset::binary::Type binaryType)
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
        virtual void AddElement(void*) const = 0;
        virtual void RemoveElementByIndex(void*, size_t) const { SPARK_ASSERT(false, "GetContainedValueByIndex - Not implemented for type."); }
    };

    struct Type_PtrBase : Type_Container
    {
        Type_PtrBase(const std::string& name,
                     const size_t size,
                     const asset::binary::Type binaryType)
            : Type_Container(name, size, binaryType)
        {}

        bool RequiresExplicitInstantiation() const override
        {
            return true;
        }

        void AddElement(void*) const override
        {
            SPARK_ASSERT(false, "Cant add element to a pointer.");
        }
    };

    template<typename T>
    struct Type_StdSharedPtr : Type_PtrBase
    {
        explicit Type_StdSharedPtr(T*)
            : Type_PtrBase{"std::shared_ptr<>", sizeof(std::shared_ptr<T>), asset::binary::Type::Bool /* GetBinaryType will redirect to contained type */ }
        {
        }

        std::string GetTypeName(const void*) const override;
        asset::binary::Type GetBinaryType() const override { return TypeResolver<T>::Get()->GetBinaryType(); }

        static Type* GetSerialisedType(const asset::binary::Object& parentObj, const std::string& fieldName);
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
            *static_cast<std::shared_ptr<T>*>(obj) = std::shared_ptr<T>(static_cast<T*>(reflect->heap_constructor()));
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
            const auto* objBase = static_cast<ObjectBase*>(typed->get());
            if (SPARK_VERIFY(objBase))
            {
                return objBase->GetTypeName();
            }

            return "";
        }

        return std::string("std::shared_ptr<") + TypeResolver<T>::Get()->GetTypeName(nullptr) + ">";
    }

    template <typename T>
    Type* Type_StdSharedPtr<T>::GetSerialisedType(const asset::binary::Object& parentObj, const std::string& fieldName)
    {
        const asset::binary::Object thisObj = fieldName.empty() ? parentObj : parentObj.Get<asset::binary::Object>(fieldName);
        const auto typeName = thisObj.GetStruct().GetName();
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
                const asset::binary::StructLayout structLayout = GetStructLayout(obj);
                const auto db = parentObj.GetDatabase();
                const auto structIndex = db->GetOrCreateStruct(objBase->GetTypeName(), structLayout);
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
        const auto* objBase = static_cast<ObjectBase*>(typed->get());
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
            if (const auto* objBase = static_cast<ObjectBase*>(typed->get()))
            {
                return objBase->GetReflectType();
            }
        }
        return TypeResolver<T>::Get();
    }

    template <typename T>
    struct TypeResolver<std::shared_ptr<T>>
    {
    public:
        static Type* Get()
        {
            static Type_StdSharedPtr<T> typeDesc{static_cast<T*>(nullptr)};
            return &typeDesc;
        }
    };

    template<typename T>
    struct Type_RawPtr : Type_PtrBase
    {
        explicit Type_RawPtr(T*)
            : Type_PtrBase{"*", sizeof(T*), asset::binary::Type::Bool /* GetBinaryType will redirect to contained type */ }
        {
        }

        std::string GetTypeName(const void*) const override;
        asset::binary::Type GetBinaryType() const override { return TypeResolver<T>::Get()->GetBinaryType(); }

        static Type* GetSerialisedType(const asset::binary::Object& parentObj, const std::string& fieldName);
        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        asset::binary::StructLayout GetStructLayout(const void*) const override;
        bool IsPolymorphic() const override { return true; }
        bool IsContainer() const override { return true; }
        std::string GetContainerTypeName() const override { return "*"; }
        Type* GetContainedValueType(const void* obj) const override;

        void* GetContainedValue(void* obj) const override
        {
            if (SPARK_VERIFY(obj))
            {
                using MutableT = std::remove_const_t<T>;
                T* constT = *static_cast<T**>(obj);
                MutableT* mutableT = const_cast<MutableT*>(constT);
                return static_cast<void*>(mutableT);
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
            *static_cast<T**>(obj) = static_cast<T*>(reflect->heap_constructor());
            return obj;
        }
    };

    template <typename T>
    std::string Type_RawPtr<T>::GetTypeName(const void* obj) const
    {
        if (obj)
        {
            static_assert(!T::s_IsPOD, "Pointer (Polymorphic) serialisation not supported for POD types.");
            auto* typed = static_cast<T* const*>(obj);
            auto* objBase = static_cast<const ObjectBase*>(*typed);
            if (SPARK_VERIFY(objBase))
            {
                return objBase->GetTypeName();
            }

            return "";
        }

        return std::string("std::shared_ptr<") + TypeResolver<T>::Get()->GetTypeName(nullptr) + ">";
    }

    template <typename T>
    Type* Type_RawPtr<T>::GetSerialisedType(const asset::binary::Object& parentObj, const std::string& fieldName)
    {
        const asset::binary::Object thisObj = fieldName.empty() ? parentObj : parentObj.Get<asset::binary::Object>(fieldName);
        const auto typeName = thisObj.GetStruct().GetName();
        return TypeFromString(typeName);
    }

    template <typename T>
    void Type_RawPtr<T>::Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        static_assert(!T::s_IsPOD, "Pointer (Polymorphic) serialisation not supported for POD types.");
        auto* typed = static_cast<T* const*>(obj);
        auto* constObj = static_cast<const ObjectBase*>(*typed);
        auto* mutableObj = const_cast<ObjectBase*>(constObj);
        if (SPARK_VERIFY(mutableObj))
        {
            if (!fieldName.empty()) // parent is an object
            {
                const asset::binary::StructLayout structLayout = GetStructLayout(obj);
                const auto db = parentObj.GetDatabase();
                const auto structIndex = db->GetOrCreateStruct(mutableObj->GetTypeName(), structLayout);
                auto binaryObj = db->CreateObject(structIndex);
                parentObj.Set(fieldName, binaryObj);
                mutableObj->Serialize(*typed, binaryObj, {});
            }
            else // parent is this
            {
                mutableObj->Serialize(*typed, parentObj, {});
            }
        }
    }

    template <typename T>
    void Type_RawPtr<T>::Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        static_assert(!T::s_IsPOD, "Pointer (Polymorphic) serialisation not supported for POD types.");
        auto* typed = static_cast<T* const*>(obj);
        auto* constObj = static_cast<const ObjectBase*>(*typed);
        auto* mutableObj = const_cast<ObjectBase*>(constObj);
        if (SPARK_VERIFY(mutableObj))
        {
            mutableObj->Deserialize(mutableObj, parentObj, fieldName);
        }
    }

    template <typename T>
    asset::binary::StructLayout Type_RawPtr<T>::GetStructLayout(const void* obj) const
    {
        static_assert(!T::s_IsPOD, "Pointer (Polymorphic) serialisation not supported for POD types.");
        auto* typed = static_cast<T* const*>(obj);
        auto* constObj = static_cast<const ObjectBase*>(*typed);
        const auto* mutableObj = const_cast<ObjectBase*>(constObj);
        if (SPARK_VERIFY(mutableObj))
        {
            return mutableObj->GetStructLayout(obj);
        }

        return {};
    }

    template<typename T>
    Type* Type_RawPtr<T>::GetContainedValueType(const void* obj) const
    {
        if (obj)
        {
            auto* typed = static_cast<T* const*>(obj);
            if (auto* objBase = static_cast<const ObjectBase*>(*typed))
            {
                return objBase->GetReflectType();
            }
        }
        return TypeResolver<T>::Get();
    }

    template <typename T>
    struct TypeResolver<T*>
    {
    public:
        static Type* Get()
        {
            static Type_RawPtr<T> typeDesc{static_cast<T*>(nullptr)};
            return &typeDesc;
        }
    };

    template <typename T> struct is_shared_ptr : std::false_type {};
    template <typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};
    template <typename T> concept SharedPtr = is_shared_ptr<T>::value;
    template <typename T> concept RawPtr = std::is_pointer_v<T>;

    template<SharedPtr T>
    T InstantiateContainerObj(const std::string& type)
    {
        const Type* reflect = TypeFromString(type);
        T obj = std::shared_ptr<typename T::element_type>(static_cast<T::element_type*>(reflect->heap_constructor()));
        return obj;
    }

    template<RawPtr T>
    T InstantiateContainerObj(const std::string& type)
    {
        const Type* reflect = TypeFromString(type);
        T obj = static_cast<T>(reflect->heap_constructor());
        return obj;
    }

    template<typename T>
    T InstantiateContainerObj(const std::string&)
    {
        T obj = {};
        return obj;
    }

    void SerializeArray(const void* obj,
                        asset::binary::Object& parentObj,
                        const std::string& fieldName,
                        const Type* itemType,
                        size_t (*getSize)(const void*), const void* (*getItem)(const void*, size_t));
    void DeserializeArray(void* obj,
                          const asset::binary::Object& parentObj, const std::string& fieldName,
                          const Type* itemType,
                          const std::function<void*(void*, size_t)>& getRawItem);
    void DeserializeVector(void* obj,
                           const asset::binary::Object& parentObj, const std::string& fieldName,
                           const Type* itemType,
                           const std::function<void*(void* vecPtr, const std::string&)>& emplace_back);

    template <typename T>
    struct Type_StdVector : Type_Container
    {
        Type* itemType;

        size_t (*getSize)(const void*);
        const void* (*getItem)(const void*, size_t);
        std::function<void*(void* vecPtr, const std::string&)> emplace_back;

        explicit Type_StdVector(T*)
            : Type_Container{"std::vector<>", sizeof(std::vector<T>), asset::binary::Type::Array},
              itemType{TypeResolver<T>::Get()}
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

            emplace_back = [](void* vecPtr, const std::string& type) -> void*
            {
                auto& vec = *static_cast<std::vector<T>*>(vecPtr);
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
        Type * GetContainedValueType(const void*) const override { return TypeResolver<T>::Get(); }
        void* GetContainedValueByIndex(void* obj, size_t i) const override;
        size_t GetNumContainedElements(void* obj) const override;
        void AddElement(void*) const override;
        void RemoveElementByIndex(void*, size_t) const override;
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
        static_cast<std::vector<T>*>(obj)->clear();
        DeserializeVector(obj, parentObj, fieldName, itemType, emplace_back);
    }

    template <typename T>
    asset::binary::StructLayout Type_StdVector<T>::GetStructLayout(const void*) const
    {
        asset::binary::StructLayout structLayout = {
            {asset::binary::CreateFixedString64("val"), GetBinaryType()}
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
            return &typed->at(i);
        }

        return nullptr;
    }

    template<typename T>
    size_t Type_StdVector<T>::GetNumContainedElements(void* obj) const
    {
        if (SPARK_VERIFY(obj))
        {
            auto* typed = static_cast<const std::vector<T>*>(obj);
            return typed->size();
        }

        return 0;
    }

    template<typename T>
    void Type_StdVector<T>::AddElement(void* obj) const
    {
        if (SPARK_VERIFY(obj))
        {
            auto* typed = static_cast<std::vector<T>*>(obj);
            typed->emplace_back(T());
        }
    }

    template<typename T>
    void Type_StdVector<T>::RemoveElementByIndex(void* obj, size_t i) const
    {
        if (SPARK_VERIFY(obj))
        {
            auto* typed = static_cast<std::vector<T>*>(obj);
            typed->erase(typed->begin() + i);
        }
    }


    template <typename T>
    struct TypeResolver<std::vector<T>>
    {
    public:
        static Type* Get()
        {
            static Type_StdVector<T> typeDesc{static_cast<T*>(nullptr)};
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

        explicit Type_StdArray(T*)
            : Type_Container{"std::array<>", sizeof(std::array<T, Size>), asset::binary::Type::Array},
              itemType{TypeResolver<T>::Get()}
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
                auto& vec = *static_cast<std::array<T, Size>*>(vecPtr);
                return &vec[index];
            };
        }

        std::string GetTypeName(const void* obj) const override;
        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        asset::binary::Type GetBinaryType() const override { return itemType->IsPolymorphic() ? asset::binary::Type::PolymorphicArray : asset::binary::Type::Array; }
        asset::binary::StructLayout GetStructLayout(const void*) const override;
        bool IsContainer() const override { return "true"; }
        std::string GetContainerTypeName() const override { return "std::array<>"; }
        Type * GetContainedValueType(const void*) const override { return itemType; }
        void* GetContainedValueByIndex(void* obj, size_t i) const override;
        size_t GetNumContainedElements(void*) const override;
        void AddElement(void*) const override;
    };

    template<typename T, size_t Size>
    std::string Type_StdArray<T, Size>::GetTypeName(const void* obj) const
    {
        return std::format("std::array<{}, {}>", itemType->GetTypeName(obj), size);
    }

    template<typename T, size_t Size>
    void Type_StdArray<T, Size>::Serialize(const void* obj,
        asset::binary::Object& parentObj,
        const std::string& fieldName) const
    {
        SerializeArray(obj, parentObj, fieldName, itemType, getSize, getItem);
    }

    template<typename T, size_t Size>
    void Type_StdArray<T, Size>::Deserialize(void* obj,
        asset::binary::Object& parentObj,
        const std::string& fieldName) const
    {
        DeserializeArray(obj, parentObj, fieldName, itemType, getRawItem);
    }

    template<typename T, size_t Size>
    asset::binary::StructLayout Type_StdArray<T, Size>::GetStructLayout(const void*) const
    {
        asset::binary::StructLayout structLayout = {
            {asset::binary::CreateFixedString64("val"), GetBinaryType() }
        };
        return structLayout;
    }

    template<typename T, size_t Size>
    void* Type_StdArray<T, Size>::GetContainedValueByIndex(void* obj,
                                                           size_t i) const
    {
        if (SPARK_VERIFY(obj))
        {
            auto* typed = static_cast<std::array<T, Size>*>(obj);
            return &typed->at(i);
        }

        return nullptr;
    }

    template<typename T, size_t Size>
    size_t Type_StdArray<T, Size>::GetNumContainedElements(void*) const
    {
        return Size;
    }

    template<typename T, size_t Size>
    void Type_StdArray<T, Size>::AddElement(void*) const
    {
        SPARK_ASSERT(false, "Can't add element to an array.");
    }

    template <typename T, size_t Size>
    struct TypeResolver<std::array<T, Size>>
    {
    public:
        static Type* Get()
        {
            static Type_StdArray<T, Size> typeDesc{static_cast<T*>(nullptr)};
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
            , keyType(TypeResolver<T>::Get())
            , itemType(TypeResolver<Y>::Get())
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
        Type * GetContainedKeyType() const override { return TypeResolver<T>::Get(); }
        Type* GetContainedValueType(const void*) const override { return TypeResolver<Y>::Get(); }
        void * GetContainedValueByIndex(void*, size_t i) const override;
        const void * GetContainedKeyByIndex(void*, size_t) const override;
        size_t GetNumContainedElements(void* obj) const override;
        void AddElement(void*) const override;
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
        const size_t numItems = getSize(obj);
        asset::binary::Database* db = parentObj.GetDatabase();

        const asset::binary::StructLayout structLayout = {
            {asset::binary::CreateFixedString64("key"), keyType->GetBinaryType() },
            {asset::binary::CreateFixedString64("value"), itemType->GetBinaryType() }
        };
        const auto* map = static_cast<const std::map<T, Y>*>(obj);
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
        std::map<T, Y>* map = static_cast<std::map<T, Y>*>(obj);
        map->clear();

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
            {asset::binary::CreateFixedString64("val"), asset::binary::Type::Array}
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
            return &it->second;
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
            return &it->first;
        }

        return nullptr;
    }

    template<typename T, typename Y>
    size_t Type_StdMap<T, Y>::GetNumContainedElements(void* obj) const
    {
        if (SPARK_VERIFY(obj))
        {
            auto* typed = static_cast<const std::map<T, Y>*>(obj);
            return typed->size();
        }

        return 0;
    }

    template<typename T, typename Y>
    void Type_StdMap<T, Y>::AddElement(void* obj) const
    {
        if (SPARK_VERIFY(obj))
        {
            auto* typed = static_cast<std::map<T, Y>*>(obj);
            typed->insert(std::make_pair(T(), Y()));
        }
    }

    template <typename T, typename Y>
    struct TypeResolver<std::map<T, Y>>
    {
    public:
        static Type* Get()
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
            , keyType(TypeResolver<T>::Get())
            , itemType(TypeResolver<Y>::Get())
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
        Type * GetContainedKeyType() const override { return TypeResolver<T>::Get(); }
        Type* GetContainedValueType(const void*) const override { return TypeResolver<Y>::Get(); }
        void* GetContainedValueByIndex(void *, size_t i) const override;
        const void * GetContainedKeyByIndex(void *, size_t) const override;
        size_t GetNumContainedElements(void* obj) const override;
        void AddElement(void*) const override;
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
        const size_t numItems = getSize(obj);
        const auto db = parentObj.GetDatabase();

        const asset::binary::StructLayout structLayout = {
            {asset::binary::CreateFixedString64("key"), keyType->GetBinaryType() },
            {asset::binary::CreateFixedString64("value"), itemType->GetBinaryType() }
        };
        auto* map = static_cast<const std::unordered_map<T, Y>*>(obj);
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
        std::unordered_map<T, Y>* map = static_cast<std::unordered_map<T, Y>*>(obj);
        map->clear();
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
            {asset::binary::CreateFixedString64("val"), asset::binary::Type::Array}
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
            return &(it->second);
        }

        return nullptr;
    }

    template<typename T, typename Y>
    const void* Type_StdUnorderedMap<T, Y>::GetContainedKeyByIndex(void* obj,
                                                    size_t i) const
    {
        if (SPARK_VERIFY(obj))
        {
            auto* typed = static_cast<std::unordered_map<T, Y>*>(obj);
            auto it = typed->begin();
            std::advance(it, i);
            return &it->first;
        }

        return nullptr;
    }

    template<typename T, typename Y>
    size_t Type_StdUnorderedMap<T, Y>::GetNumContainedElements(void* obj) const
    {
        if (SPARK_VERIFY(obj))
        {
            auto* typed = static_cast<const std::unordered_map<T, Y>*>(obj);
            return typed->size();
        }

        return 0;
    }

    template<typename T, typename Y>
    void Type_StdUnorderedMap<T, Y>::AddElement(void* obj) const
    {
        if (SPARK_VERIFY(obj))
        {
            auto* typed = static_cast<std::unordered_map<T, Y>*>(obj);
            typed->insert(std::make_pair(T(), Y()));
        }
    }

    template <typename T, typename Y>
    struct TypeResolver<std::unordered_map<T, Y>>
    {
    public:
        static Type* Get()
        {
            static Type_StdUnorderedMap<T, Y> typeDesc{nullptr, nullptr};
            return &typeDesc;
        }
    };
}
