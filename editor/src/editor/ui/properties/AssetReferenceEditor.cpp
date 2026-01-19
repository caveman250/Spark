#include "AssetReferenceEditor.h"

#include "engine/ecs/SceneSaveData.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(asset::AssetReference<asset::Texture>, AssetReferenceEditor<asset::Texture>, AssetReference_Texture);
    DEFINE_PROPERTY_EDITOR(asset::AssetReference<asset::Shader>, AssetReferenceEditor<asset::Shader>, AssetReference_Shader);
    DEFINE_PROPERTY_EDITOR(asset::AssetReference<asset::Model>, AssetReferenceEditor<asset::Model>, AssetReference_Model);
    DEFINE_PROPERTY_EDITOR(asset::AssetReference<asset::Font>, AssetReferenceEditor<asset::Font>, AssetReference_Font);
    DEFINE_PROPERTY_EDITOR(asset::AssetReference<ecs::SceneSaveData>, AssetReferenceEditor<ecs::SceneSaveData>, AssetReference_SceneSaveData);
    DEFINE_PROPERTY_EDITOR(asset::AssetReference<render::Material>, AssetReferenceEditor<render::Material>, AssetReference_Material);
}
