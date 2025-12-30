#pragma once

#include "AssetManager.h"
#include "spark.h"
#include "engine/reflect/Reflect.h"
// #include "font/Font.h"
// #include "mesh/Model.h"
// #include "shader/Shader.h"
// #include "texture/Texture.h"

namespace se::render
{
    class Material;
}

namespace se::asset
{
    class Font;
    class Model;
    class Shader;
    class Texture;

    template <typename T>
    class AssetReference : public reflect::ObjectBase
    {
        SPARK_CLASS_TEMPLATED()

        AssetReference() = default;
        AssetReference(const std::string& path);
        bool IsSet() const { return !m_AssetPath.empty(); }
        bool Loaded() const { return m_Instance.get(); }
        const std::shared_ptr<T>& GetAsset();

    private:
        SPARK_MEMBER(Serialized)
        std::string m_AssetPath = {};

        SPARK_MEMBER()
        std::shared_ptr<T> m_Instance = nullptr;
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
    const std::shared_ptr<T>& AssetReference<T>::GetAsset()
    {
        if (!m_Instance)
        {
            auto assetManager = AssetManager::Get();
            m_Instance = assetManager->GetAsset<T>(m_AssetPath);
        }

        return m_Instance;
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