#include <editor/ui/properties/util/PropertyUtil.h>
#include "MapEditor.h"

#include "engine/Application.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/VerticalBoxComponent.h"
#include "engine/asset/AssetManager.h"
#include "engine/io/VFS.h"
#include "engine/render/Material.h"
#include "engine/render/MaterialInstance.h"
#include "engine/ui/components/TextComponent.h"

namespace se::editor::ui::properties
{
    DEFINE_CONTAINER_PROPERTY_EDITOR("std::map<>", MapEditor)
    DEFINE_CONTAINER_PROPERTY_EDITOR("std::unordered_map<>", MapEditor)

    void MapEditor::SetValue(void* value, const reflect::Type* type)
    {
        if (SPARK_VERIFY(type->IsContainer()))
        {
            m_VectorType = static_cast<const reflect::Type_Container*>(type);
            m_Value = value;
        }
    }

    void MapEditor::ConstructUI(const PropertyEditorParams& params)
    {
        PropertyEditor::ConstructUI(params);

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();
        auto assetManager = asset::AssetManager::Get();

        auto listBG = world->CreateEntity(editor->GetEditorScene(), "Vector Editor BG");
        auto* listRect = world->AddComponent<RectTransformComponent>(listBG);
        world->AddComponent<WidgetComponent>(listBG);
        listRect->anchors = {0.f, 1.f, 0.f, 0.f };
        listRect->minX = 5;
        listRect->maxX = 5;
        auto listBGImage = world->AddComponent<ImageComponent>(listBG);
        std::shared_ptr<render::Material> bgMaterial = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_containerbg.sass");
        listBGImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(bgMaterial);
        world->AddChild(m_Content, listBG);

        ecs::Id verticalBoxEntity = world->CreateEntity(editor->GetEditorScene(), "Vector Editor Vertical Box");
        auto* verticalBoxRect = world->AddComponent<RectTransformComponent>(verticalBoxEntity);
        verticalBoxRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        world->AddComponent<WidgetComponent>(verticalBoxEntity);
        auto verticalBox = world->AddComponent<VerticalBoxComponent>(verticalBoxEntity);
        verticalBox->spacing = 5;
        world->AddChild(listBG, verticalBoxEntity);

        reflect::Type* stringType = reflect::TypeResolver<std::string>::Get();
        size_t numElements = m_VectorType->GetNumContainedElements(m_Value);
        if (numElements == 0)
        {
            auto textEntity = world->CreateEntity(editor->GetEditorScene(), "Title");
            world->AddComponent<WidgetComponent>(textEntity);
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->font = "/engine_assets/fonts/Arial.sass";
            text->fontSize = 14;
            text->text = "empty.";
            auto textRect = world->AddComponent<RectTransformComponent>(textEntity);
            textRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
            textRect->minX = 5;
            world->AddChild(verticalBoxEntity, textEntity);
        }
        else
        {
            for (size_t i = 0; i < numElements; ++i)
            {
                void* obj = m_VectorType->GetContainedValueByIndex(m_Value, i);
                auto containedType = m_VectorType->GetContainedValueType(obj);

                std::string propName = std::format("{}", i);
                if (m_VectorType->GetContainedKeyType() == stringType)
                {
                    const void* value = m_VectorType->GetContainedKeyByIndex(m_Value, i);
                    propName = *((std::string*)value);
                }

                auto entity = world->CreateEntity(editor->GetEditorScene(), propName);
                auto rect = world->AddComponent<RectTransformComponent>(entity);
                rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
                rect->minX = 2;
                rect->maxX = 15;
                world->AddComponent<WidgetComponent>(entity);

                PropertyEditorParams propertyParams = {
                    .name = propName + ": " + containedType->GetTypeName(m_VectorType->GetContainedValueByIndex(m_Value, i)),
                    .type = containedType,
                    .value = obj,
                    .anchors = se::ui::Anchors(0.f, 1.f, 0.f, 0.f),
                    .collapsed = true,
                    .withBackground = false,
                    .constructTitle = true
                };
                auto propertyEditor = CreatePropertyEditor(propertyParams);
                if (!propertyEditor)
                {
                    auto text = util::CreateMissingPropertyEditorText(containedType, 0.f, 0);
                    world->AddChild(entity, text);
                }
                else
                {
                    world->AddChild(entity, propertyEditor->GetWidgetId());
                    m_Editors.push_back(propertyEditor);
                }

                world->AddChild(verticalBoxEntity, entity);
            }
        }
    }

    void MapEditor::Update()
    {
        for (auto* editor : m_Editors)
        {
            editor->Update();
        }
    }
}
