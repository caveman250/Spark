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
#include "engine/ui/components/ButtonComponent.h"
#include "editor/Transactions.h"

namespace se::editor::ui::properties
{
    DEFINE_CONTAINER_PROPERTY_EDITOR("std::vector<>", VectorEditor)

    std::shared_ptr<VectorEditor> s_StaticInstance = nullptr;

    VectorEditor::~VectorEditor()
    {
    }

    void VectorEditor::SetValue(void* value, const reflect::Type* type)
    {
        if (SPARK_VERIFY(type->IsContainer()))
        {
            m_VectorType = static_cast<const reflect::Type_Container*>(type);
            m_Value = value;
        }
    }

    void VectorEditor::ConstructUI(const PropertyEditorParams& params)
    {
        PropertyEditor::ConstructUI(params);

        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditor();
        auto assetManager = asset::AssetManager::Get();

        auto listBG = world->CreateEntity(editor->GetEditorScene(), "Vector Editor BG");
        auto* listRect = world->AddComponent<RectTransformComponent>(listBG);
        world->AddComponent<WidgetComponent>(listBG);
        listRect->anchors = { 0.f, 1.f, 0.f, 0.f };
        listRect->minX = 5;
        listRect->maxX = 5;
        auto listBGImage = world->AddComponent<ImageComponent>(listBG);
        std::shared_ptr<render::Material> bgMaterial = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_containerbg.sass");
        listBGImage->materialInstance = std::make_shared<render::MaterialInstance>(bgMaterial);
        world->AddChild(m_Content, listBG);

        m_VerticalBox = world->CreateEntity(editor->GetEditorScene(), "Vector Editor Vertical Box");
        auto* verticalBoxRect = world->AddComponent<RectTransformComponent>(m_VerticalBox);
        verticalBoxRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        world->AddComponent<WidgetComponent>(m_VerticalBox);
        auto verticalBox = world->AddComponent<VerticalBoxComponent>(m_VerticalBox);
        verticalBox->spacing = 5;
        world->AddChild(listBG, m_VerticalBox);

        auto buttonContainer = world->CreateEntity(editor->GetEditorScene(), "New Button Container");
        auto* buttonRect = world->AddComponent<RectTransformComponent>(buttonContainer);
        world->AddComponent<WidgetComponent>(buttonContainer);
        buttonRect->anchors = { 0.f, 1.f, 0.f, 0.f };
        world->AddChild(m_VerticalBox, buttonContainer);

        auto newButtonEntity = world->CreateEntity(editor->GetEditorScene(), "New Button");
        world->AddComponent<WidgetComponent>(newButtonEntity);
        auto newButton = world->AddComponent<ButtonComponent>(newButtonEntity);
        newButton->image = "/engine_assets/textures/editor_plus.sass";
        newButton->pressedImage = "/engine_assets/textures/editor_plus.sass";
        newButton->hoveredImage = "/engine_assets/textures/editor_plus.sass";
        newButton->onReleased.Subscribe([this](input::MouseButton, bool)
        {
            size_t newIndex = m_VectorType->GetNumContainedElements(m_Value);
            Transactions::Get()->PushAction([this]()
            {
                m_VectorType->AddElement(m_Value);
            },
            [this, newIndex]()
            {
                m_VectorType->RemoveElementByIndex(m_Value, newIndex);
            });

        });
        auto newButtonRect = world->AddComponent<RectTransformComponent>(newButtonEntity);
        newButtonRect->anchors = { .left = 1.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        newButtonRect->minX = 15;
        newButtonRect->maxY = 15;
        world->AddChild(buttonContainer, newButtonEntity);

        CreateElements();
        m_CachedVectorSize = m_VectorType->GetNumContainedElements(m_Value);
    }

    void VectorEditor::Update()
    {
        for (const auto& editor : m_Editors)
        {
            editor->Update();
        }

        size_t vectorSize = m_VectorType->GetNumContainedElements(m_Value);
        if (m_CachedVectorSize != vectorSize)
        {
            RecreateElements();
            m_CachedVectorSize = vectorSize;
        }
    }

    std::string VectorEditor::CreateEditorName(size_t i, reflect::Type* type) const
    {
        return std::format("{}: {}", i, type->GetTypeName(m_VectorType->GetContainedValueByIndex(m_Value, i)));
    }

    void VectorEditor::RecreateElements()
    {
        DestroyElements();
        CreateElements();

        auto* transform = Application::Get()->GetWorld()->GetComponent<RectTransformComponent>(m_VerticalBox);
        transform->needsLayout = true;
        se::ui::util::InvalidateParent(m_VerticalBox, nullptr);
    }

    void VectorEditor::CreateElements()
    {
        auto* app = Application::Get();
        auto* editor = app->GetEditor();
        auto* world = app->GetWorld();
        size_t numElements = m_VectorType->GetNumContainedElements(m_Value);
        if (numElements == 0)
        {
            m_EmptyItem = world->CreateEntity(editor->GetEditorScene(), "Title");
            world->AddComponent<WidgetComponent>(m_EmptyItem);
            auto text = world->AddComponent<TextComponent>(m_EmptyItem);
            text->font = "/engine_assets/fonts/CascadiaCode.sass";
            text->fontSize = 16;
            text->text = "empty.";
            auto textRect = world->AddComponent<RectTransformComponent>(m_EmptyItem);
            textRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
            textRect->minX = 2;
            textRect->minY = 1;
            textRect->maxY = 22;
            world->AddChild(m_VerticalBox, m_EmptyItem);
        }
        else
        {
            for (size_t i = 0; i < m_VectorType->GetNumContainedElements(m_Value); ++i)
            {
                InstantiateElementUI(i);
            }
        }
    }

    void VectorEditor::DestroyElements()
    {
        auto* world = Application::Get()->GetWorld();
        if (m_EmptyItem != ecs::InvalidEntity)
        {
            world->DestroyEntity(m_EmptyItem);
            m_EmptyItem = ecs::InvalidEntity;
        }

        for (const auto& editor : m_Editors)
        {
            editor->DestroyUI();
        }
        m_Editors.clear();
    }

    void VectorEditor::InstantiateElementUI(size_t i)
    {
        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditor();

        void* obj = m_VectorType->GetContainedValueByIndex(m_Value, i);
        auto containedType = m_VectorType->GetContainedValueType(obj);

        auto propName = std::format("{}", i);
        auto entity = world->CreateEntity(editor->GetEditorScene(), propName);
        auto rect = world->AddComponent<RectTransformComponent>(entity);
        rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        rect->minX = 2;
        rect->maxX = 15;
        world->AddComponent<WidgetComponent>(entity);

        PropertyEditorParams propertyParams = {
            .name = CreateEditorName(i, containedType),
            .type = containedType,
            .value = obj,
            .anchors = se::ui::Anchors(0.f, 1.f, 0.f, 1.f),
            .collapsed = true,
            .withBackground = false,
            .constructTitle = true,
            .contextOptions = {
                std::make_pair("Delete", [this, obj]()
                {
                    auto it = std::ranges::find_if(m_Editors,
                        [obj](const auto& propEd){ return propEd->GetValue() == obj; });
                    if (SPARK_VERIFY(it != m_Editors.end()))
                    {
                        size_t index = it - m_Editors.begin();
                        std::any oldVal = m_VectorType->GetContainedValueByIndexCopy(m_Value, index);

                        Transactions::Get()->PushAction([weakthis = weak_from_this(), index, vectorType = m_VectorType, value = m_Value]()
                        {
                            vectorType->RemoveElementByIndex(value, index);
                        },
                        [weakthis = weak_from_this(), vectorType = m_VectorType, value = m_Value, index, oldVal]()
                        {
                            vectorType->AddElementAtIndex(value, oldVal, index);
                        });
                    }
                })
            }
         };
        auto propertyEditor = CreatePropertyEditor(propertyParams);
        if (!propertyEditor)
        {
            auto text = util::CreateMissingPropertyEditorText(containedType, 0.f, 0);
            world->AddChild(entity, text);
        }
        else if (propertyEditor->GetWidgetId() != ecs::InvalidEntity)
        {
            world->AddChild(entity, propertyEditor->GetWidgetId());
            m_Editors.insert(m_Editors.begin() + i, propertyEditor);
        }

        world->InsertChild(m_VerticalBox, entity, i + 1);
    }

    void VectorEditor::DestroyElementUI(const size_t i)
    {
        const auto& editor = m_Editors[i];
        editor->DestroyUI();
        m_Editors.erase(m_Editors.begin() + i);
    }
}
