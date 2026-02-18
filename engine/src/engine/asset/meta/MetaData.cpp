#include "MetaData.h"

namespace se::asset::meta
{
    MetaData::MetaData(const std::string& assetPath)
        : m_AssetPath(assetPath)
    {
    }

    uint32_t MetaData::GetFormatVersion() const
    {
        return m_Version;
    }

    void MetaData::SetFormatVersion(const uint32_t version)
    {
        m_Version = version;
    }
}
