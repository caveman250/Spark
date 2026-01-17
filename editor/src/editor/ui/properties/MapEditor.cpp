#include <editor/ui/properties/util/PropertyUtil.h>
#include "MapEditor.h"

#include "engine/Application.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/VerticalBoxComponent.h"
#include "engine/asset/AssetManager.h"
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

    void MapEditor::ConstructUI(const std::string& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground)
    {
        PropertyEditor::ConstructUI(name, constructTitle, anchors, collapsed, withBackground);

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();

        static std::shared_ptr<render::Material> bgMaterial = nullptr;
        if (!bgMaterial)
        {
            bgMaterial = std::make_shared<render::Material>(
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/ui.sass") },
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/flat_color.sass") });
            bgMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.27f, 0.27f, 0.27f));
        }

        auto listBG = world->CreateEntity(editor->GetEditorScene(), "Vector Editor BG");
        auto* listRect = world->AddComponent<se::ui::components::RectTransformComponent>(listBG);
        world->AddComponent<se::ui::components::WidgetComponent>(listBG);
        listRect->anchors = {0.f, 1.f, 0.f, 0.f };
        listRect->minX = 5;
        listRect->maxX = 5;
        auto listBGImage = world->AddComponent<se::ui::components::ImageComponent>(listBG);
        listBGImage->materialInstance = se::render::MaterialInstance::CreateMaterialInstance(bgMaterial);
        world->AddChild(m_Content, listBG);

        ecs::Id verticalBoxEntity = world->CreateEntity(editor->GetEditorScene(), "Vector Editor Vertical Box");
        auto* verticalBoxRect = world->AddComponent<se::ui::components::RectTransformComponent>(verticalBoxEntity);
        verticalBoxRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        world->AddComponent<se::ui::components::WidgetComponent>(verticalBoxEntity);
        auto verticalBox = world->AddComponent<se::ui::components::VerticalBoxComponent>(verticalBoxEntity);
        verticalBox->spacing = 5;
        world->AddChild(listBG, verticalBoxEntity);

        reflect::Type* stringType = reflect::TypeResolver<std::string>::get();
        size_t numElements = m_VectorType->GetNumContainedElements(m_Value);
        if (numElements == 0)
        {
            auto textEntity = world->CreateEntity(editor->GetEditorScene(), "Title");
            world->AddComponent<WidgetComponent>(textEntity);
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
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

                auto propertyEditor = CreatePropertyEditor(propName + "    (" + containedType->GetTypeName(m_VectorType->GetContainedValueByIndex(m_Value, i)) + ")",
                                                           containedType,
                                                           obj,
                                                           se::ui::Anchors(0.f, 1.f, 0.f, 0.f),
                                                           true,
                                                           false,
                                                           true);
                if (!propertyEditor)
                {
                    auto text = properties::util::CreateMissingPropertyEditorText(containedType, 0.f, 0);
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
