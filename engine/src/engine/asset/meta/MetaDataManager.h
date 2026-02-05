#pragma once

#include "MetaData.h"
#include "TextureMetaData.h"
#include "spark.h"
#include "engine/asset/Asset.h"
#include "engine/asset/util/AssetUtil.h"

namespace se::asset::meta
{
    class MetaManager
    {
    public:
        static MetaManager* Get();

        static std::string GetMetaPath(std::string assetPath);

        std::shared_ptr<MetaData> GetOrCreateMetaDataForAsset(Asset* asset);

        template <typename T>
        std::shared_ptr<T> GetOrCreateMetaDataForAsset(std::string assetPath)
        {
            auto it = m_MetaCache.find(assetPath);
            if (it != m_MetaCache.end())
            {
                return std::static_pointer_cast<T>(it->second);
            }

            auto metaPath = GetMetaPath(assetPath);
            if (metaPath.empty())
            {
                return nullptr;
            }

            auto ret = std::make_shared<T>(assetPath);
            InitMetaData(ret);

            return ret;
        }

        void SaveMetaData(const std::shared_ptr<MetaData>& meta);

    private:
        void InitMetaData(const std::shared_ptr<MetaData>& meta);

        std::unordered_map<std::string, std::shared_ptr<MetaData>> m_MetaCache;
    };
}
