#pragma once

namespace se::asset::meta
{
    class MetaData
    {
    public:
        static std::optional<MetaData> GetMetaDataForAsset(const std::string& assetPath);
        static MetaData CreateMetaDataForAsset(const std::string& assetPath);
        void Save();

        uint32_t GetFormatVersion() const;
        void SetFormatVersion(uint32_t version);

    private:
        MetaData(const std::string& assetPath);

        nlohmann::json ToJson();
        void FromJson(const nlohmann::json& json);
        static std::string GetMetaPath(const std::string& assetPath);

        uint32_t m_Version = 0;

        std::string m_AssetPath = {};
    };
}
