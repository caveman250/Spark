#pragma once

#include "AssetManager.h"
#include "spark.h"
#include "engine/reflect/Reflect.h"

namespace se::asset
{
    template <typename T>
    class AssetReference : public reflect::ObjectBase
    {
        SPARK_CLASS_TEMPLATED()

        AssetReference() = default;
        AssetReference(const std::string& path);
        AssetReference(const char* path);
        bool IsSet() const { return !m_AssetPath.empty(); }
        bool Loaded() const { return m_Instance.get(); }
        const std::shared_ptr<T>& GetAsset() const;

        void operator=(const std::string& path);
        void operator=(const char* path);

    private:
        SPARK_MEMBER(Serialized)
        std::string m_AssetPath = {};

        mutable std::shared_ptr<T> m_Instance = nullptr;
    };

    SPARK_INSTANTIATE_TEMPLATE(AssetReference, Font);
    SPARK_INSTANTIATE_TEMPLATE(AssetReference, Model);
    SPARK_INSTANTIATE_TEMPLATE(AssetReference, Shader);
    SPARK_INSTANTIATE_TEMPLATE(AssetReference, Texture);
    SPARK_INSTANTIATE_TEMPLATE(AssetReference, render::Material);

    template<typename T>
    AssetReference<T>::AssetReference(const std::string& path)
        : m_AssetPath(path)
    {
    }

    template<typename T>
    AssetReference<T>::AssetReference(const char* path)
         : m_AssetPath(path)
    {

    }

    template<typename T>
    const std::shared_ptr<T>& AssetReference<T>::GetAsset() const
    {
        if (!m_Instance)
        {
            auto assetManager = AssetManager::Get();
            m_Instance = assetManager->GetAsset<T>(m_AssetPath);
        }

        return m_Instance;
    }

    template<typename T>
    void AssetReference<T>::operator=(const std::string& path)
    {
        *this = AssetReference(path);
    }

    template<typename T>
    void AssetReference<T>::operator=(const char* path)
    {
        *this = AssetReference(path);
    }

    template <typename T>
    static std::vector<std::shared_ptr<T>> LoadAssetList(std::vector<AssetReference<T>>& references)
    {
        std::vector<std::shared_ptr<T>> ret = {};
        ret.reserve(references.size());
        for (auto& asset : references)
        {
            ret.push_back(asset.GetAsset());
        }

        return ret;
    }
}

#include "se_asset_AssetReference.generated.h"