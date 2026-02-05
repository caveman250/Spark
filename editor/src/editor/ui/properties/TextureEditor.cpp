#include "TextureEditor.h"

#include <engine/Application.h>
#include <engine/ui/components/RectTransformComponent.h>

#include "engine/asset/meta/MetaDataManager.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/util/SplitViewUtil.h"
#include "util/PropertyUtil.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(se::asset::Texture, TextureEditor, Texture);

    void TextureEditor::SetValue(void* value, const reflect::Type*)
    {
        m_Value = static_cast<asset::Texture*>(value);
    }

    void TextureEditor::ConstructUI(const std::string& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground)
    {
        auto metaManager = asset::meta::MetaManager::Get();
        auto meta = metaManager->GetOrCreateMetaDataForAsset(m_Value);
        auto editor = Application::Get()->GetEditorRuntime();



        auto metaType = meta->GetReflectType();
        m_WrappedEditor = CreatePropertyEditor(name,
                                               metaType,
                                               meta.get(),
                                               anchors,
                                               collapsed,
                                               withBackground,
                                               constructTitle);

        auto world = Application::Get()->GetWorld();

        auto previewContainer = world->CreateEntity(editor->GetEditorScene(), "Preview Container");
        auto previewContainerRect = world->AddComponent<RectTransformComponent>(previewContainer);
        previewContainerRect->minHeight = std::min(m_Value->GetHeight(), 256u);
        world->AddComponent<WidgetComponent>(previewContainer);

        auto previewInnerContainer = world->CreateEntity(editor->GetEditorScene(), "Preview Inner Container");
        auto previewInnerContainerRect = world->AddComponent<RectTransformComponent>(previewInnerContainer);
        previewInnerContainerRect->minY = 10;
        previewInnerContainerRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        world->AddComponent<WidgetComponent>(previewInnerContainer);
        auto image = world->AddComponent<ImageComponent>(previewInnerContainer);
        auto imageMaterial = asset::AssetManager::Get()->GetAsset<render::Material>("/engine_assets/materials/editor_darkbg.sass");
        image->materialInstance = render::MaterialInstance::CreateMaterialInstance(imageMaterial);
        world->AddChild(previewContainer, previewInnerContainer);

        auto preview = world->CreateEntity(editor->GetEditorScene(), "Preview");
        auto previewRect = world->AddComponent<RectTransformComponent>(preview);
        previewRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        previewRect->minAspectRatio = m_Value->GetWidth() / m_Value->GetHeight();
        previewRect->maxAspectRatio = m_Value->GetWidth() / m_Value->GetHeight();
        auto previewImage = world->AddComponent<ImageComponent>(preview);
        previewImage->texture = m_Value->m_Path;
        world->AddChild(previewInnerContainer, preview);

        world->AddChild(m_WrappedEditor->GetContentId(), previewContainer);
    }

    void TextureEditor::Update()
    {
    }
}
