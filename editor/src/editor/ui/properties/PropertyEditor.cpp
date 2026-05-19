#include "PropertyEditor.h"

#include "engine/ecs/Signal.h"
#include "ClassEditor.h"
#include "EnumEditor.h"
#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/CollapsingHeaderComponent.h"
#include "engine/ui/components/EditableTextComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/VerticalBoxComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/text/WrapMode.h"
#include "engine/ui/util/CollapsingHeaderUtil.h"
#include "engine/ui/util/ContextMenuUtil.h"
#include "engine/ui/util/EditableTextUtil.h"

namespace se::ui::components
{
    struct WidgetComponent;
}

namespace se::editor::ui::properties
{
    std::unordered_map<std::string, reflect::Type*>& GetContainerPropertyEditorTypes()
    {
        static std::unordered_map<std::string, reflect::Type*> s_Instance = { };
        return s_Instance;
    }

    std::unordered_map<reflect::Type*, reflect::Type*>& GetPropertyEditorTypes()
    {
        static std::unordered_map<reflect::Type*, reflect::Type*> s_Instance = { };
        return s_Instance;
    }

    void PropertyEditor::ConstructUI(const PropertyEditorParams& params)
    {
        constexpr int titleFontSize = 14;
        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditor();

        m_Name = params.name;

        m_TitleMode = params.titleModeOverride != PropertyTitleMode::None ?
            params.titleModeOverride :
            GetDefaultTitleMode();

        ecs::Id titleContainer = { };
        if (GetTitleMode() == PropertyTitleMode::NextLine)
        {
            ecs::Id contentsEntity;
            CollapsingHeaderComponent* collapsingHeader = nullptr;
            m_WidgetId = se::ui::util::CreateCollapsingHeader(world, m_Name, titleContainer, contentsEntity, &collapsingHeader, params.collapsed, params.withBackground, editor->GetEditorScene());
            m_RectTransform = world->GetComponent<RectTransformComponent>(m_WidgetId);
            m_RectTransform->anchors = params.anchors;

            if (!params.contextOptions.empty())
            {
                auto contextButton = world->CreateEntity(editor->GetEditorScene(), "Context Button");
                auto button = world->AddComponent<ButtonComponent>(contextButton);
                button->image = "/engine_assets/textures/editor_context.sass";
                button->pressedImage = "/engine_assets/textures/editor_context.sass";
                button->hoveredImage = "/engine_assets/textures/editor_context.sass";
                button->onReleased.Subscribe([world, editor, options = params.contextOptions](input::MouseButton)
                {
                    auto inputComp = world->GetSingletonComponent<input::InputComponent>();
                    se::ui::util::ContextMenuParams contextParams = {
                        .fontSize = 14,
                        .minWidth = 200,
                        .mousePos = { inputComp->mouseX, inputComp->mouseY },
                        .scene = editor->GetEditorScene(),
                        .options = options
                    };

                    se::ui::util::CreateContextMenu(contextParams);
                });
                auto buttonTrans = world->AddComponent<RectTransformComponent>(contextButton);
                buttonTrans->anchors = { .left = 1.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
                buttonTrans->minX = 18;
                buttonTrans->maxX = 2;
                buttonTrans->maxY = 18;
                buttonTrans->minY = 2;
                world->AddChild(titleContainer, contextButton);
            }

            m_Content = world->CreateEntity(editor->GetEditorScene(), std::format("Property Content ({})", params.name));
            auto contentRect = world->AddComponent<RectTransformComponent>(m_Content);
            contentRect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
            contentRect->minX = 5;
            world->AddComponent<WidgetComponent>(m_Content);
            auto verticalBox = world->AddComponent<VerticalBoxComponent>(m_Content);
            verticalBox->spacing = 10;
            world->AddChild(contentsEntity, m_Content);
        }
        else
        {
            m_WidgetId = world->CreateEntity(editor->GetEditorScene(), params.name);
            m_RectTransform = world->AddComponent<RectTransformComponent>(m_WidgetId);
            world->AddComponent<MouseInputComponent>(m_WidgetId);
            m_RectTransform->anchors = params.anchors;
            world->AddComponent<WidgetComponent>(m_WidgetId);
            titleContainer = m_WidgetId;
            m_Content = m_WidgetId;
        }

        if (params.constructTitle)
        {
            if (params.editableTitle)
            {
                auto newText = se::ui::util::CreateEditableText(world, "/engine_assets/fonts/CascadiaCode.sass", titleFontSize, editor->GetEditorScene());
                m_Title = newText.entity;
                newText.text->text = m_Name;
                newText.text->wrap = se::ui::text::WrapMode::Char;
                se::ui::util::SetEditTextMouseInputEnabled(newText.mouseInput, false);
            }
            else
            {
                m_Title = world->CreateEntity(editor->GetEditorScene(), std::format("Property Title ({})", params.name));
                auto titleText = world->AddComponent<TextComponent>(m_Title);
                titleText->font = "/engine_assets/fonts/CascadiaCode.sass";
                titleText->fontSize = titleFontSize;
                titleText->text = m_Name;
                titleText->wrap = se::ui::text::WrapMode::Char;
            }

            auto titleRect = world->AddComponent<RectTransformComponent>(m_Title);
            titleRect->anchors = {
                .left = 0.f,
                .right = GetTitleMode() == PropertyTitleMode::Inline ? 0.3f : 1.f,
                .top = 0.f,
                .bottom = 1.f
            };


            world->AddChild(titleContainer, m_Title);

            if (GetTitleMode() == PropertyTitleMode::Inline && !params.contextOptions.empty())
            {
                auto buttonEntity = world->CreateEntity(editor->GetEditorScene(), "Property Invisible Button");
                auto buttonRect = world->AddComponent<RectTransformComponent>(buttonEntity);
                buttonRect->anchors = {
                    .left = 0.f,
                    .right = 1.f,
                    .top = 0.f,
                    .bottom = 1.f
                };
                auto button = world->AddComponent<ButtonComponent>(buttonEntity);
                button->onReleased.Subscribe([world, editor, options = params.contextOptions](input::MouseButton mouseButton)
                {
                    if (mouseButton == input::MouseButton::Right)
                    {
                        auto inputComp = world->GetSingletonComponent<input::InputComponent>();
                        se::ui::util::ContextMenuParams contextParams = {
                            .fontSize = 14,
                            .minWidth = 200,
                            .mousePos = { inputComp->mouseX, inputComp->mouseY },
                            .scene = editor->GetEditorScene(),
                            .options = options
                        };

                        se::ui::util::CreateContextMenu(contextParams);
                    }
                });
                auto widget = world->AddComponent<WidgetComponent>(buttonEntity);
                widget->visibility = se::ui::Visibility::Hidden;
                world->AddChild(m_Title, buttonEntity);
            }
        }
    }

    void PropertyEditor::DestroyUI()
    {
        Application::Get()->GetWorld()->DestroyEntity(m_WidgetId);
    }

    void PropertyEditor::UpdateName(const std::string& name)
    {
        m_Name = name;
        auto text = Application::Get()->GetWorld()->GetComponent<TextComponent>(m_Title);
        text->text = name;

    }

    PropertyTitleMode PropertyEditor::GetTitleMode() const
    {
        return m_TitleMode;
    }

    void PropertyEditor::BeginRename(const std::string_view editText,
                                     const std::function<void(const std::string&, EditableTextComponent*)>& onComitted,
                                     const std::function<void(EditableTextComponent*)>& onCancelled)
    {
        const auto world = Application::Get()->GetWorld();
        if (GetTitleMode() == PropertyTitleMode::NextLine)
        {
            const CollapsingHeaderComponent* collapsingHeader = world->GetComponent<CollapsingHeaderComponent>(GetWidgetId());
            WidgetComponent* buttonWidget = world->GetComponent<WidgetComponent>(collapsingHeader->titleButton);
            EditableTextComponent* text = world->GetComponent<EditableTextComponent>(GetTitleId());
            KeyInputComponent* keyInput = world->GetComponent<KeyInputComponent>(GetTitleId());

            se::ui::util::BeginEditingText(nullptr, GetTitleId(), *text, *keyInput);
            text->editText = editText;
            auto* mouseInput = world->GetComponent<se::ui::components::MouseInputComponent>(GetTitleId());
            se::ui::util::SetEditTextMouseInputEnabled(mouseInput, true);
            se::ui::util::SetCaretPos(*text, static_cast<int>(text->editText.size()));

            buttonWidget->updateEnabled = false;
            buttonWidget->visibility = se::ui::Visibility::Collapsed;

            m_RenameComittedHandle = text->onComitted.Subscribe([this, world, onComitted](std::string newText)
            {
                EditableTextComponent* text = world->GetComponent<EditableTextComponent>(GetTitleId());
                KeyInputComponent* keyInput = world->GetComponent<KeyInputComponent>(GetTitleId());
                MouseInputComponent* mouseInput = world->GetComponent<MouseInputComponent>(GetTitleId());
                se::ui::util::EndEditingText(nullptr, GetTitleId(), *text, *keyInput);
                se::ui::util::SetEditTextMouseInputEnabled(mouseInput, false);
                onComitted(newText, text);
                text->onComitted.Unsubscribe(m_RenameComittedHandle);
                text->onCancelled.Unsubscribe(m_RenameCancelledHandle);
            });

            m_RenameCancelledHandle = text->onCancelled.Subscribe([this, world, onCancelled]()
            {
                EditableTextComponent* text = world->GetComponent<EditableTextComponent>(GetTitleId());
                KeyInputComponent* keyInput = world->GetComponent<KeyInputComponent>(GetTitleId());
                MouseInputComponent* mouseInput = world->GetComponent<MouseInputComponent>(GetTitleId());
                se::ui::util::EndEditingText(nullptr, GetTitleId(), *text, *keyInput);
                se::ui::util::SetEditTextMouseInputEnabled(mouseInput, false);

                onCancelled(text);
                text->onComitted.Unsubscribe(m_RenameComittedHandle);
                text->onCancelled.Unsubscribe(m_RenameCancelledHandle);
            });
        }
        else
        {

        }
    }

    std::shared_ptr<PropertyEditor> CreatePropertyEditor(const PropertyEditorParams& params)
    {
        reflect::Type* editor_type = nullptr;

        if (params.type->IsEnum())
        {
            editor_type = EnumEditor::GetReflection();
        }
        else if (params.type->IsContainer())
        {
            auto* container = static_cast<reflect::Type_Container*>(params.type);
            editor_type = GetContainerPropertyEditorTypes()[container->GetContainerTypeName()];
        }
        else
        {
            editor_type = GetPropertyEditorTypes()[params.type];
        }

        if (!editor_type)
        {
            if (params.type->IsClass())
            {
                editor_type = reflect::TypeResolver<ClassEditor>::Get();
            }
            else
            {
                return nullptr;
            }
        }

        auto editor = std::shared_ptr<PropertyEditor>(static_cast<PropertyEditor*>(editor_type->heap_constructor()));
        editor->SetValue(params.value, params.type);
        editor->SetName(params.name);
        editor->ConstructUI(params);
        return editor;
    }
}
