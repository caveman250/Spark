#include <editor/ui/properties/util/PropertyUtil.h>
#include "VectorEditor.h"

#include "engine/Application.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/VerticalBoxComponent.h"
#include "engine/ui/util/VerticalBoxUtil.h"
#include "engine/asset/AssetManager.h"
#include "engine/render/Material.h"
#include "engine/render/MaterialInstance.h"

namespace se::editor::ui::properties
{
    DEFINE_CONTAINER_PROPERTY_EDITOR("std::vector<>", VectorEditor)

    DEFINE_SPARK_CLASS_BEGIN(VectorEditor)
    DEFINE_SPARK_CLASS_END(VectorEditor)

    void VectorEditor::SetValue(void* value, const reflect::Type* type)
    {
        if (SPARK_VERIFY(type->IsContainer()))
        {
            m_VectorType = static_cast<const reflect::Type_Container*>(type);
            m_Value = value;
        }
    }

    void VectorEditor::ConstructUI(const String& name, bool constructTitle)
    {
        PropertyEditor::ConstructUI(name, constructTitle);

        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();

        static std::shared_ptr<render::Material> bgMaterial = nullptr;
        if (!bgMaterial)
        {
            auto vert = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/ui.sass");
            auto frag = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/flat_color.sass");
            bgMaterial = render::Material::CreateMaterial({vert}, {frag});
            bgMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(0.23f, 0.23f, 0.23f));
        }

        auto listBG = world->CreateEntity("Vector Editor BG", true);
        auto* listRect = world->AddComponent<se::ui::components::RectTransformComponent>(listBG);
        listRect->anchors = { 0.f, 1.f, 0.f, 1.f };
        listRect->minX = 5;
        listRect->minY = constructTitle ? 27 : 5;
        listRect->maxX = 5;
        auto listBGImage = world->AddComponent<se::ui::components::ImageComponent>(listBG);
        listBGImage->materialInstance = se::render::MaterialInstance::CreateMaterialInstance(bgMaterial);
        world->AddChild(m_WidgetId, listBG);

        ecs::Id verticalBoxEntity = world->CreateEntity("Vector Editor Vertical Box", true);
        auto* verticalBoxRect = world->AddComponent<se::ui::components::RectTransformComponent>(verticalBoxEntity);
        verticalBoxRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        verticalBoxRect->overridesChildSizes = true;
        world->AddComponent<se::ui::components::WidgetComponent>(verticalBoxEntity);
        auto verticalBox = world->AddComponent<se::ui::components::VerticalBoxComponent>(verticalBoxEntity);
        verticalBox->spacing = 5;
        world->AddChild(listBG, verticalBoxEntity);

        auto containedType = m_VectorType->GetContainedValueType();
        for (size_t i = 0; i < m_VectorType->GetNumContainedElements(m_Value); ++i)
        {
            auto propertyEditor = CreatePropertyEditor(std::format("{}", i),
                                                    containedType,
                                                    m_VectorType->GetContainedValueByIndex(m_Value, i));
            if (!propertyEditor)
            {
                auto entity = world->CreateEntity(name, true);
                auto rect = world->AddComponent<RectTransformComponent>(entity);
                rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
                rect->minX = 5;
                rect->maxX = 15;
                world->AddComponent<WidgetComponent>(entity);

                auto titleEntity = world->CreateEntity("Title", true);
                auto titleText = world->AddComponent<TextComponent>(titleEntity);
                titleText->font = asset::AssetManager::Get()->GetAsset<asset::Font>("/engine_assets/fonts/Arial.sass");
                titleText->fontSize = 18;
                titleText->text = std::format("{}", i);
                auto titleRect = world->AddComponent<RectTransformComponent>(titleEntity);
                titleRect->anchors = { .left = 0.f, .right = 0.5f, .top = 0.f, .bottom = 0.f };
                titleRect->minX = 2;
                titleRect->minY = 1;
                titleRect->maxY = 22;
                world->AddChild(entity, titleEntity);

                auto text = properties::util::CreateMissingPropertyEditorText(containedType, .5f);
                world->AddChild(entity, text);

                se::ui::util::AddVerticalBoxChild(verticalBoxEntity, verticalBox, entity);
            }
            else
            {
                se::ui::util::AddVerticalBoxChild(verticalBoxEntity, verticalBox, propertyEditor->GetWidgetId());
            }
        }
    }

    void VectorEditor::Update()
    {
    }
}
