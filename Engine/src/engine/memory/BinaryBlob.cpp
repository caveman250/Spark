#include "BinaryBlob.h"

#include "engine/asset/binary/Object.h"

namespace se::reflect
{
    BinaryBlob::BinaryBlob(void(* init)(BinaryBlob*))
        : Type(nullptr, 0, asset::binary::Type::Blob)
    {
        init(this);
    }

    asset::binary::StructLayout BinaryBlob::GetStructLayout(const void*) const
    {
        asset::binary::StructLayout structLayout = {{ asset::binary::CreateFixedString32("val"), asset::binary::Type::Blob }};
        return structLayout;
    }

    void BinaryBlob::Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        if (!fieldName.empty())
        {
            const auto* blob = static_cast<const memory::BinaryBlob*>(obj);
            auto assetBlob = parentObj.GetDatabase()->CreateBlob(static_cast<const char*>(blob->GetData()), blob->GetSize());
            parentObj.Set(fieldName, assetBlob);
        }
        else
        {
            const auto* blob = static_cast<const memory::BinaryBlob*>(obj);
            auto assetBlob = parentObj.GetDatabase()->CreateBlob(static_cast<const char*>(blob->GetData()), blob->GetSize());
            parentObj.Set("val", assetBlob);
        }
    }

    void BinaryBlob::Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const
    {
        if (!fieldName.empty())
        {
            auto binaryObj = parentObj.Get<asset::binary::Blob>(fieldName);
            void* newData = malloc(binaryObj.GetSize());
            memcpy(newData, binaryObj.GetData(), binaryObj.GetSize());
            memory::BinaryBlob* blob = (memory::BinaryBlob*)obj;
            blob->SetData(newData);
            blob->SetSize(binaryObj.GetSize());
        }
        else
        {
            auto binaryObj = parentObj.Get<asset::binary::Blob>("val");
            void* newData = malloc(binaryObj.GetSize());
            memcpy(newData, binaryObj.GetData(), binaryObj.GetSize());
            memory::BinaryBlob* blob = (memory::BinaryBlob*)obj;
            blob->SetData(newData);
            blob->SetSize(binaryObj.GetSize());
        }
    }
}

namespace se::memory
{
    DEFINE_SPARK_TYPE(BinaryBlob)
    se::reflect::BinaryBlob BinaryBlob::Reflection{BinaryBlob::initReflection};
    void BinaryBlob::initReflection(reflect::BinaryBlob* typeDesc)
    {
        typeDesc->name ="BinaryBlob";
        typeDesc->size = sizeof(BinaryBlob);
        typeDesc->heap_constructor = []{ return new BinaryBlob(); };
        typeDesc->inplace_constructor = [](void* mem){ return new(mem) BinaryBlob(); };
        typeDesc->heap_copy_constructor = [](void* other){ return new BinaryBlob(*static_cast<BinaryBlob*>(other)); };
        typeDesc->inplace_copy_constructor = [](void* mem, void* other){ return new(mem) BinaryBlob(*static_cast<BinaryBlob*>(other)); };
        typeDesc->destructor = [](void* data){ static_cast<BinaryBlob*>(data)->~BinaryBlob(); };
    }

    void BinaryBlob::Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName)
    {
        reflect::TypeResolver<BinaryBlob>::get()->Serialize(obj, parentObj, fieldName);
    }

    void BinaryBlob::Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName)
    {
        reflect::TypeResolver<BinaryBlob>::get()->Deserialize(obj, parentObj, fieldName);
    }

    asset::binary::StructLayout BinaryBlob::GetStructLayout(const void* obj) const
    {
        return reflect::TypeResolver<BinaryBlob>::get()->GetStructLayout(obj);
    }

    std::string BinaryBlob::GetTypeName() const
    {
        return "BinaryBlob";
    }

    BinaryBlob::BinaryBlob()
    {
    }

    BinaryBlob::BinaryBlob(void* data, size_t size)
        : m_Data(data)
        , m_Size(size)
    {
    }

    void BinaryBlob::Release()
    {
        free(m_Data);
        m_Data = nullptr;
    }
}
