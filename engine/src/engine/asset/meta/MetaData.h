#pragma once
#include "engine/io/VFS.h"
#include "engine/reflect/Object.h"

namespace se::asset::meta
{
    class MetaData : public reflect::ObjectBase
    {
        SPARK_CLASS()
    public:
        const std::string& GetAssetPath() { return m_AssetPath; }
        uint32_t GetFormatVersion() const;
        void SetFormatVersion(uint32_t version);

        MetaData(const std::string& assetPath);
    protected:

        SPARK_MEMBER(Serialized)
        uint32_t m_Version = 0;

        std::string m_AssetPath = {};
    };
}
