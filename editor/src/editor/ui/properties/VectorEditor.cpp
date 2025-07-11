#include <editor/ui/properties/util/PropertyUtil.h>
#include "VectorEditor.h"

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
    DEFINE_CONTAINER_PROPERTY_EDITOR("std::vector<>", VectorEditor)

    void VectorEditor::SetValue(void* value, const reflect::Type* type)
    {
        if (SPARK_VERIFY(type->IsContainer()))
        {
            m_VectorType = static_cast<const reflect::Type_Container*>(type);
            m_Value = value;
        }
    }

    void VectorEditor::ConstructUI(const String& name, bool constructTitle, const se::ui::Anchors& anchors, bool collapsed, bool withBackground)
    {
        PropertyEditor::ConstructUI(name, constructTitle, anchors, collapsed, withBackground);

        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();

        static std::shared_ptr<render::Material> bgMaterial = nullptr;
        if (!bgMaterial)
        {
            auto vert = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/ui.sass");
            auto frag = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/flat_color.sass");
            bgMaterial = render::Material::CreateMaterial({vert}, {frag});
            bgMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.27f, 0.27f, 0.27f));
        }

        auto listBG = world->CreateEntity("Vector Editor BG", true);
        auto* listRect = world->AddComponent<RectTransformComponent>(listBG);
        world->AddComponent<WidgetComponent>(listBG);
        listRect->anchors = { 0.f, 1.f, 0.f, 0.f };
        listRect->minX = 5;
        listRect->maxX = 5;
        auto listBGImage = world->AddComponent<ImageComponent>(listBG);
        listBGImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(bgMaterial);
        world->AddChild(m_Content, listBG);

        ecs::Id verticalBoxEntity = world->CreateEntity("Vector Editor Vertical Box", true);
        auto* verticalBoxRect = world->AddComponent<RectTransformComponent>(verticalBoxEntity);
        verticalBoxRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        verticalBoxRect->overridesChildSizes = true;
        world->AddComponent<WidgetComponent>(verticalBoxEntity);
        auto verticalBox = world->AddComponent<VerticalBoxComponent>(verticalBoxEntity);
        verticalBox->spacing = 5;
        world->AddChild(listBG, verticalBoxEntity);

        size_t numElements = m_VectorType->GetNumContainedElements(m_Value);
        if (numElements == 0)
        {
            auto textEntity = world->CreateEntity("Title", true);
            world->AddComponent<WidgetComponent>(textEntity);
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
            text->fontSize = 16;
            text->text = "empty.";
            auto textRect = world->AddComponent<RectTransformComponent>(textEntity);
            textRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
            textRect->minX = 2;
            textRect->minY = 1;
            textRect->maxY = 22;
            world->AddChild(verticalBoxEntity, textEntity);
        }
        else
        {
            for (size_t i = 0; i < m_VectorType->GetNumContainedElements(m_Value); ++i)
            {
                void* obj = m_VectorType->GetContainedValueByIndex(m_Value, i);
                auto containedType = m_VectorType->GetContainedValueType(obj);

                auto propName = std::format("{}", i);
                auto entity = world->CreateEntity(propName, true);
                auto rect = world->AddComponent<RectTransformComponent>(entity);
                rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
                rect->minX = 5;
                rect->maxX = 15;
                world->AddComponent<WidgetComponent>(entity);

                auto titleEntity = world->CreateEntity("Title", true);
                auto titleText = world->AddComponent<TextComponent>(titleEntity);
                titleText->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
                titleText->fontSize = 16;
                titleText->text = propName;
                world->AddComponent<WidgetComponent>(titleEntity);
                auto titleRect = world->AddComponent<RectTransformComponent>(titleEntity);
                titleRect->anchors = {.left = 0.f, .right = 0.3f, .top = 0.f, .bottom = 0.f};
                titleRect->minX = 2;
                titleRect->minY = 1;
                titleRect->maxY = 22;
                world->AddChild(entity, titleEntity);

                auto propertyEditor = CreatePropertyEditor(containedType->GetTypeName(m_VectorType->GetContainedValueByIndex(m_Value, i)),
                                                           containedType,
                                                           obj,
                                                           se::ui::Anchors(0.35f, 1.f, 0.f, 0.f),
                                                           true,
                                                           false,
                                                           true);
                if (!propertyEditor)
                {
                    auto text = util::CreateMissingPropertyEditorText(containedType, .35f, 0);
                    world->AddChild(entity, text);
                }
                else
                {
                    world->AddChild(entity, propertyEditor->GetWidgetId());
                }

                world->AddChild(verticalBoxEntity, entity);
            }
        }
    }

    void VectorEditor::Update()
    {
    }
}
