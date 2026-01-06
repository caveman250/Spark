#pragma once

#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::reflect
{
    struct BinaryBlob : Type
    {
        BinaryBlob() : Type("BinaryBlob", 0, asset::binary::Type::Blob) {}
        asset::binary::StructLayout GetStructLayout(const void* obj) const override;
        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
    };
}

namespace se::memory
{
    class BinaryBlob : public reflect::ObjectBase
    {
    public:
        static constexpr bool s_IsPOD = false;
        static reflect::Type* GetReflection();
        static size_t s_StaticId;

        reflect::Type* GetReflectType() const override;
        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) override;
        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) override;
        asset::binary::StructLayout GetStructLayout(const void* obj) const override;
        std::string GetTypeName() const override;
        void Invoke(const std::string&, const std::vector<std::any>&) override {}

        BinaryBlob();
        BinaryBlob(void* data, size_t size);
        void Release();

        void SetData(void* val) { m_Data = val; }
        void SetSize(size_t val) { m_Size = val; }
        void* GetData() const { return m_Data; }
        size_t GetSize() const { return m_Size; }
    private:
        void* m_Data = nullptr;
        size_t m_Size = 0;
    };
}
