#include "AssetManager.h"
#include "Asset.h"
#include "engine/Application.h"
#include "engine/render/MaterialInstance.h"
#include "engine/render/singleton_components/MeshRenderComponent.h"

namespace se::asset
{
    AssetManager* AssetManager::Get()
    {
        static AssetManager* s_Instance = new AssetManager();
        return s_Instance;
    }

    std::shared_ptr<Asset> AssetManager::GetAsset(const std::string& path,
                                                  reflect::Type* type)
    {
        std::lock_guard guard(m_Mutex);

        const auto it = m_AssetCache.find(path);
        if (it != m_AssetCache.end())
        {
            if (type == reflect::TypeResolver<render::MaterialInstance>::Get())
            {
                return std::make_shared<render::MaterialInstance>(*static_cast<render::MaterialInstance*>(it->second.get()));
            }

            return it->second;
        }

        const auto db = binary::Database::Load(path, true);
        if (!SPARK_VERIFY(db))
        {
            return nullptr;
        }

        auto root = db->GetRoot();
        std::shared_ptr<Asset> asset = std::shared_ptr<Asset>(static_cast<Asset*>(type->heap_constructor()));
        asset->m_Path = path;
        type->Deserialize(asset.get(), root, {});
        m_AssetCache.insert(std::make_pair(path,asset));

        if (type == reflect::TypeResolver<render::MaterialInstance>::Get())
        {
            return std::make_shared<render::MaterialInstance>(*static_cast<render::MaterialInstance*>(asset.get()));
        }

        return asset;
    }

    void AssetManager::ReleaseAsset(const std::string& path)
    {
        auto it = m_AssetCache.find(path);
        if (it != m_AssetCache.end())
        {
            m_AssetCache.erase(it);
        }
    }

#if SPARK_EDITOR
    void AssetManager::ForceReloadAsset(const std::string& path, reflect::Type* type)
    {
        if (m_AssetCache.contains(path))
        {
            auto& asset = m_AssetCache.at(path);
            const auto db = binary::Database::Load(path, true);
            if (!SPARK_VERIFY(db))
            {
                return;
            }
            auto root = db->GetRoot();
            type->Deserialize(asset.get(), root, {});
            auto* meshRenderComp = Application::Get()->GetWorld()->GetSingletonComponent<render::singleton_components::MeshRenderComponent>();
            meshRenderComp->invalidatedMeshAssets.push_back(asset::AssetReference<Model>(path));
        }
    }
#endif
}
