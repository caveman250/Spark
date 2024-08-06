#include "AssetManager.h"
#include "Asset.h"

namespace se::asset
{
    AssetManager* AssetManager::Get()
    {
        static AssetManager* s_Instance = new AssetManager();
        return s_Instance;
    }
}
