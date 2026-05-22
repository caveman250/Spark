#include "AssetBrowserUtil.h"

#include "engine/Application.h"
#include "engine/ecs/SceneSaveData.h"

namespace se::editor::ui::asset_browser
{
    void SelectFile(const std::string& file)
    {
        auto app = Application::Get();
        Editor* runtime = app->GetEditor();
        auto db = asset::binary::Database::Load(file, true);

        std::shared_ptr<asset::Asset> asset = asset::AssetManager::Get()->GetAsset(file,
                                                                                   reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));

        runtime->SelectAsset(asset);
    }

    void OpenFile(const std::string& file)
    {
        auto app = Application::Get();
        Editor* runtime = app->GetEditor();
        auto db = asset::binary::Database::Load(file, true);

        std::shared_ptr<asset::Asset> asset = asset::AssetManager::Get()->GetAsset(file,
                                                                                   reflect::TypeFromString(db->GetRoot().GetStruct().GetName()));

        if (asset->GetReflectType() == ecs::SceneSaveData::GetReflection())
        {
            runtime->LoadScene(file);
        }
        else if (asset->GetReflectType() == ecs::Prefab::GetReflection())
        {
            runtime->EditPrefab(asset);
        }
    }

    void SelectFolder(const std::string&)
    {
        // TODO
    }
}
