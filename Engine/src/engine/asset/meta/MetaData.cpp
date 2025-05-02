#include "MetaData.h"

#include "engine/io/VFS.h"

namespace se::asset::meta
{
    std::optional<MetaData> MetaData::GetMetaDataForAsset(const std::string& assetPath)
    {
        if (!SPARK_VERIFY(!assetPath.ends_with(".sass"), "MetaData::GetMetaDataForAsset - Not supported for built assets."))
        {
            return std::nullopt;
        }

        auto metaPath = GetMetaPath(assetPath);
        if (metaPath.empty())
        {
            return std::nullopt;
        }

        auto ret = CreateMetaDataForAsset(assetPath);
        if (!io::VFS::Get().Exists(metaPath))
        {
            return ret;
        }
        else
        {
            String metaData = io::VFS::Get().ReadText(metaPath);
            nlohmann::json json = nlohmann::json::parse(metaData.Data());
            ret.FromJson(json);
            return ret;
        }
    }

    MetaData MetaData::CreateMetaDataForAsset(const std::string& assetPath)
    {
        return MetaData(assetPath); // assuming this will get more complicated?
    }

    void MetaData::Save()
    {
        nlohmann::json saveData = ToJson();
        io::VFS::Get().WriteText(GetMetaPath(m_AssetPath), saveData.dump());
    }

    MetaData::MetaData(const std::string& assetPath)
        : m_AssetPath(assetPath)
    {
    }

    nlohmann::json MetaData::ToJson()
    {
        nlohmann::json meta;
        meta["version"] = m_Version;
        return meta;
    }

    void MetaData::FromJson(const nlohmann::json& json)
    {
        if (json.contains("version"))
        {
            m_Version = json["version"].get<uint32_t>();
        }
    }

    std::string MetaData::GetMetaPath(const std::string& assetPath)
    {
        auto metaPath = assetPath;
        auto extensionIndex = metaPath.find_last_of('.');
        if (extensionIndex != std::string::npos)
        {
            auto first = metaPath.find(".");
            auto size = metaPath.size() - first;
            metaPath.replace(first, first + size, ".meta");
        }
        else
        {
            debug::Log::Error("MetaData::GetMetaDataForAsset - invalid filename {}.", assetPath);
            return {};
        }

        return metaPath;
    }

    uint32_t MetaData::GetFormatVersion() const
    {
        return m_Version;
    }

    void MetaData::SetFormatVersion(uint32_t version)
    {
        m_Version = version;
    }
}
