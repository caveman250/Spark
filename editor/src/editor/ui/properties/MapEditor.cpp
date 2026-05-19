#include "MapEditor.h"
#include "editor/Transactions.h"
#include "editor/ui/properties/util/PropertyUtil.h"
#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/io/VFS.h"
#include "engine/render/Material.h"
#include "engine/render/MaterialInstance.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/CollapsingHeaderComponent.h"
#include "engine/ui/components/EditableTextComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/VerticalBoxComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/util/ContextMenuUtil.h"

namespace se::editor::ui::properties
{
    DEFINE_CONTAINER_PROPERTY_EDITOR("std::map<>", MapEditor)
    DEFINE_CONTAINER_PROPERTY_EDITOR("std::unordered_map<>", MapEditor)

    void MapEditor::SetValue(void* value, const reflect::Type* type)
    {
        if (SPARK_VERIFY(type->IsContainer()))
        {
            m_MapType = static_cast<const reflect::Type_Container*>(type);
            m_Value = value;
        }
    }

    void MapEditor::ConstructUI(const PropertyEditorParams& params)
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
        newButton->onReleased.Subscribe([this, world](input::MouseButton)
        {
            auto inputComp = world->GetSingletonComponent<input::InputComponent>();
            const auto& derivedTypes = m_MapType->GetContainedValueType(nullptr)->GetDerivedTypes();
            se::ui::util::ContextMenuParams params = {
                .fontSize = 14,
                .mousePos = { inputComp->mouseX, inputComp->mouseY },
                .scene = Application::Get()->GetEditor()->GetEditorScene()
            };
            for (auto* type : derivedTypes)
            {
                params.AddOption(type->GetTypeName(nullptr), [this, world, type]()
                {
                    std::any key = m_MapType->GetNextKey(m_Value);
                    Transactions::Get()->PushAction([this, world, type]()
                    {
                        std::any key = m_MapType->AddElement(m_Value, type);
                        InstantiateElementUI(key, m_MapType->GetContainedValueByKey(m_Value, key));
                        auto* transform = world->GetComponent<RectTransformComponent>(m_VerticalBox);
                        transform->needsLayout = true;
                        se::ui::util::InvalidateParent(m_VerticalBox, nullptr);
                    },
                    [this, key, world]()
                    {
                        m_MapType->RemoveElementByKey(m_Value, key);
                        const auto keyStr = std::any_cast<std::string>(key);
                        for (const auto& [entity, name] : m_ElementNames)
                        {
                            if (name == keyStr)
                            {
                                world->DestroyEntity(entity);
                                auto it = m_Editors.find(entity);
                                m_Editors.erase(it);
                                m_ElementNames.erase(entity);
                                break;
                            }
                        }
                        auto* transform = world->GetComponent<RectTransformComponent>(m_VerticalBox);
                        transform->needsLayout = true;
                        se::ui::util::InvalidateParent(m_VerticalBox, nullptr);
                    });
                });
            }

            se::ui::util::CreateContextMenu(params);
        });
        auto newButtonRect = world->AddComponent<RectTransformComponent>(newButtonEntity);
        newButtonRect->anchors = { .left = 1.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        newButtonRect->minX = 15;
        newButtonRect->maxY = 15;
        world->AddChild(buttonContainer, newButtonEntity);

        size_t numElements = m_MapType->GetNumContainedElements(m_Value);
        if (numElements == 0)
        {
            auto textEntity = world->CreateEntity(editor->GetEditorScene(), "Title");
            world->AddComponent<WidgetComponent>(textEntity);
            auto text = world->AddComponent<TextComponent>(textEntity);
            text->font = "/engine_assets/fonts/CascadiaCode.sass";
            text->fontSize = 14;
            text->text = "empty.";
            auto textRect = world->AddComponent<RectTransformComponent>(textEntity);
            textRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
            textRect->minX = 5;
            world->AddChild(m_VerticalBox, textEntity);
        }
        else
        {
            for (size_t i = 0; i < numElements; ++i)
            {
                InstantiateElementUI(i);
            }
        }
    }

    void MapEditor::Update()
    {
        for (const auto& editor : m_Editors | std::ranges::views::values)
        {
            editor->Update();
        }
    }

    void MapEditor::InstantiateElementUI(size_t i)
    {
        reflect::Type* stringType = reflect::TypeResolver<std::string>::Get();
        std::string propName = std::format("{}", i);
        if (m_MapType->GetContainedKeyType() == stringType)
        {
            const void* value = m_MapType->GetContainedKeyByIndex(m_Value, i);
            propName = *(std::string*)value;
        }

        InstantiateElementUI(propName, m_MapType->GetContainedValueByIndex(m_Value, i));
    }

    void MapEditor::InstantiateElementUI(const std::any& key, void* element)
    {
        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditor();

        // only supporting string keys until I have a need for other types.
        const auto& propName = std::any_cast<std::string>(key);

        auto containedType = m_MapType->GetContainedValueType(element);

        auto entity = world->CreateEntity(editor->GetEditorScene(), propName);
        auto rect = world->AddComponent<RectTransformComponent>(entity);
        rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        rect->minX = 2;
        rect->maxX = 15;
        world->AddComponent<WidgetComponent>(entity);

        PropertyEditorParams propertyParams = {
            .name = propName + ": " + containedType->GetTypeName(element),
            .type = containedType,
            .value = element,
            .anchors = se::ui::Anchors(0.f, 1.f, 0.f, 0.f),
            .collapsed = true,
            .withBackground = false,
            .constructTitle = true,
            .titleModeOverride = PropertyTitleMode::NextLine,
            .editableTitle = m_MapType->GetContainedKeyType() == reflect::TypeResolver<std::string>::Get(),
            .contextOptions = {
                std::make_pair("Rename", [this, world, entity]()
                {
                    auto it = m_Editors.find(entity);
                    if (it != m_Editors.end())
                    {
                        const auto nameIt = m_ElementNames.find(entity);
                        std::string oldVal = nameIt->second;

                        auto onComitted = [this, world, entity, oldVal](const std::string& newText, EditableTextComponent*)
                        {
                            auto it = m_Editors.find(entity);
                            const CollapsingHeaderComponent* collapsingHeader = world->GetComponent<CollapsingHeaderComponent>(it->second->GetWidgetId());
                            WidgetComponent* buttonWidget = world->GetComponent<WidgetComponent>(collapsingHeader->titleButton);
                            buttonWidget->updateEnabled = true;
                            buttonWidget->visibility = se::ui::Visibility::Hidden;

                            Transactions::Get()->PushAction([this, entity, newText, world]()
                            {
                                const auto nameIt = m_ElementNames.find(entity);
                                m_MapType->ChangeKey(m_Value, nameIt->second, newText);
                                const void* element = m_MapType->GetContainedValueByKey(m_Value, newText);
                                nameIt->second = newText;

                                auto it = m_Editors.find(entity);
                                EditableTextComponent* text = world->GetComponent<EditableTextComponent>(it->second->GetTitleId());
                                text->text = nameIt->second + ": " + m_MapType->GetContainedValueType(element)->GetTypeName(element);
                            },
                            [this, entity, oldVal, world]()
                            {
                                const auto nameIt = m_ElementNames.find(entity);
                                m_MapType->ChangeKey(m_Value, nameIt->second, oldVal);
                                const void* element = m_MapType->GetContainedValueByKey(m_Value, oldVal);
                                nameIt->second = oldVal;

                                auto it = m_Editors.find(entity);
                                EditableTextComponent* text = world->GetComponent<EditableTextComponent>(it->second->GetTitleId());
                                text->text = nameIt->second + ": " + m_MapType->GetContainedValueType(element)->GetTypeName(element);
                            });
                        };

                        auto onCancelled = [this, world, entity](EditableTextComponent* textComp)
                        {
                            const auto nameIt = m_ElementNames.find(entity);
                            const void* element = m_MapType->GetContainedValueByKey(m_Value, nameIt->second);
                            textComp->text = nameIt->second + ": " + m_MapType->GetContainedValueType(element)->GetTypeName(element);

                            auto it = m_Editors.find(entity);
                            const CollapsingHeaderComponent* collapsingHeader = world->GetComponent<CollapsingHeaderComponent>(it->second->GetWidgetId());
                            WidgetComponent* buttonWidget = world->GetComponent<WidgetComponent>(collapsingHeader->titleButton);
                            buttonWidget->updateEnabled = true;
                            buttonWidget->visibility = se::ui::Visibility::Hidden;
                        };

                        it->second->BeginRename(nameIt->second, onComitted, onCancelled);
                    }
                }),
                std::make_pair("Delete", [this, entity, world]
                {
                    const auto nameIt = m_ElementNames.find(entity);
                    std::any oldKey = nameIt->second;
                    std::any oldVal = m_MapType->GetContainedValueByKeyCopy(m_Value, oldKey);

                    Transactions::Get()->PushAction([this, entity, world]()
                    {
                        const auto nameIt = m_ElementNames.find(entity);
                        m_MapType->RemoveElementByKey(m_Value, nameIt->second);
                        m_ElementNames.erase(nameIt);
                        auto editor = m_Editors.find(entity);
                        editor->second->DestroyUI();
                        m_Editors.erase(editor);

                        auto* transform = world->GetComponent<RectTransformComponent>(m_VerticalBox);
                        transform->needsLayout = true;
                        se::ui::util::InvalidateParent(m_VerticalBox, nullptr);
                    },
                    [this, oldKey, oldVal, world, entity]()
                    {
                        std::any key = m_MapType->AddElement(m_Value, oldVal);
                        m_MapType->ChangeKey(m_Value, key, oldKey);
                        InstantiateElementUI(oldKey, m_MapType->GetContainedValueByKey(m_Value, oldKey));

                        auto* transform = world->GetComponent<RectTransformComponent>(m_VerticalBox);
                        transform->needsLayout = true;
                        se::ui::util::InvalidateParent(m_VerticalBox, nullptr);
                    });

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
            m_Editors.insert(std::make_pair(entity, propertyEditor));
            m_ElementNames.insert(std::make_pair(entity, propName));
        }

        world->AddChild(m_VerticalBox, entity);
    }
}
