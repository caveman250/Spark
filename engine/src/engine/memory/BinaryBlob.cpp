#include "BinaryBlob.h"

#include "engine/asset/binary/Object.h"

namespace se::reflect
{
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
            auto assetBlob = parentObj.GetDatabase()->CreateBlob(static_cast<const char*>(blob->GetData()), static_cast<uint32_t>(blob->GetSize()));
            parentObj.Set(fieldName, assetBlob);
        }
        else
        {
            const auto* blob = static_cast<const memory::BinaryBlob*>(obj);
            auto assetBlob = parentObj.GetDatabase()->CreateBlob(static_cast<const char*>(blob->GetData()), static_cast<uint32_t>(blob->GetSize()));
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

    reflect::Type* BinaryBlob::GetReflection()
    {
        static reflect::BinaryBlob* s_Instance = nullptr;
        if (!s_Instance)
        {
            s_Instance = new reflect::BinaryBlob();
            s_Instance->name ="BinaryBlob";
            s_Instance->size = sizeof(BinaryBlob);
            s_Instance->heap_constructor = []{ return new BinaryBlob(); };
            s_Instance->inplace_constructor = [](void* mem){ return new(mem) BinaryBlob(); };
            s_Instance->heap_copy_constructor = [](void* other){ return new BinaryBlob(*static_cast<BinaryBlob*>(other)); };
            s_Instance->inplace_copy_constructor = [](void* mem, void* other){ return new(mem) BinaryBlob(*static_cast<BinaryBlob*>(other)); };
            s_Instance->destructor = [](void* data){ static_cast<BinaryBlob*>(data)->~BinaryBlob(); };
        }

        return s_Instance;
    }

    reflect::Type* BinaryBlob::GetReflectType() const
    {
        return reflect::TypeResolver<BinaryBlob>::get();
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
