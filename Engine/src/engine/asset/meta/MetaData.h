#pragma once

namespace se::asset::meta
{
    class MetaData
    {
    public:
        static std::optional<MetaData> GetMetaDataForAsset(const std::string& assetPath);
        static MetaData CreateMetaDataForAsset(const std::string& assetPath);
        void Save();
    private:
        MetaData(const std::string& assetPath);

        nlohmann::json ToJson();
        void FromJson(const nlohmann::json& json);
        static std::string GetMetaPath(const std::string& assetPath);

        std::string m_AssetPath = {};
    };
}
