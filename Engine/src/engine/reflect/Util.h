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
        db->SetRootStruct(db->GetOrCreateStruct(reflect->GetStructLayout()));
        auto root = db->GetRoot();
        reflect->Serialize(obj, root, {});
        return db;
    }

    template<typename T>
    T DeserialiseType(const std::shared_ptr<asset::binary::Database>& db)
    {
        T obj;
        const Type* reflect = TypeResolver<T>::get();
        auto root = db->GetRoot();
        reflect->Deserialize(&obj, root, {});
        return obj;
    }
}