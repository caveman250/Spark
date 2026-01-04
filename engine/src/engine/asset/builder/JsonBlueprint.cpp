#include "JsonBlueprint.h"

#include "engine/io/VFS.h"

namespace se::asset::builder
{
    std::regex JsonBlueprint::GetFilePattern() const
    {
        return std::regex(".*.json");
    }

    std::vector<BuiltAsset> JsonBlueprint::BuildAsset(const std::string& path,
        const std::string& outputPath,
        meta::MetaData& meta) const
    {
        std::vector<BuiltAsset> ret;

        std::string jsonText = io::VFS::Get().ReadText(path);
        nlohmann::json json = nlohmann::json::parse(jsonText);
        auto db = binary::Database::FromJson(json);
        ret.push_back({ db, "" });

        return ret;
    }
}
