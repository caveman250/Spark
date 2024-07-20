#pragma once

#include "spark.h"

namespace se::reflect
{
    struct BinaryBlob : Type
    {
        BinaryBlob(void (*init)(BinaryBlob*));

        asset::binary::StructLayout GetStructLayout() const override;
        void Serialize(const void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
        void Deserialize(void* obj, asset::binary::Object& parentObj, const std::string& fieldName) const override;
    };
}

namespace se::memory
{
    class BinaryBlob
    {
    public:
        static reflect::BinaryBlob Reflection;
        static void initReflection(reflect::BinaryBlob*);
        DECLARE_SPARK_TYPE(BinaryBlob)

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
