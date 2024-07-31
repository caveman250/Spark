#pragma once

#include "spark.h"
#include "engine/asset/binary/Database.h"

namespace se::reflect
{
    template<typename T>
    std::shared_ptr<asset::binary::Database> SerialiseType(void* obj)
    {
        const Type* reflect = TypeResolver<T>::get();
        auto db = asset::binary::Database::Create(false);
        db->SetRootStruct(db->GetOrCreateStruct(reflect->GetTypeName(obj), reflect->GetStructLayout(obj)));
        auto root = db->GetRoot();
        reflect->Serialize(obj, root, {});
        return db;
    }

    template<SharedPtr T>
    T DeserialiseType(const std::shared_ptr<asset::binary::Database>& db)
    {
        auto root = db->GetRoot();
        const Type* reflect = Type_StdSharedPtr<typename T::element_type>::GetSerialisedType(root, {});
        T obj = std::shared_ptr<typename T::element_type>(static_cast<typename T::element_type*>(reflect->heap_constructor()));
        reflect->Deserialize(obj.get(), root, {});
        return obj;
    }

    template<NotSharedPtr T>
    T DeserialiseType(const std::shared_ptr<asset::binary::Database>& db)
    {
        T obj;
        const Type* reflect = TypeResolver<T>::get();
        auto root = db->GetRoot();
        reflect->Deserialize(&obj, root, {});
        return obj;
    }
}