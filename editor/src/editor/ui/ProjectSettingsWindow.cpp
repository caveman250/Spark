#include "ProjectSettingsWindow.h"
#include "engine/Application.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/ui/components/ButtonComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/components/TextComponent.h"
#include "engine/ui/components/TitleBarComponent.h"
#include "engine/ui/components/VerticalBoxComponent.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/WindowComponent.h"
#include "engine/ui/util/ContextMenuUtil.h"
#include "engine/ui/util/ScrollBoxUtil.h"
#include "engine/ui/util/WindowUtil.h"
#include "properties/util/PropertyUtil.h"

namespace se::editor::ui
{
    void ProjectSettingsWindow::Update()
    {
        ToolWindow::Update();

        for (const auto& editor : m_PropertyEditors)
        {
            editor->Update();
        }
    }

    void ProjectSettingsWindow::ConstructUI()
    {
        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditor();

        m_InputBlocker = world->CreateEntity(editor->GetEditorScene(), "Input Blocker");
        world->AddComponent<se::ui::components::WidgetComponent>(m_InputBlocker);
        auto mouseInput = world->AddComponent<se::ui::components::MouseInputComponent>(m_InputBlocker);
        mouseInput->buttonMask = 0xFFFFFFFF;
        auto keyInput = world->AddComponent<se::ui::components::KeyInputComponent>(m_InputBlocker);
        keyInput->keyMask = input::Key::Unknown;
        auto blockerTransform = world->AddComponent<se::ui::components::RectTransformComponent>(m_InputBlocker);
        blockerTransform->anchors = {0.f, 1.f, 0.f, 1.f};
        const auto image = world->AddComponent<se::ui::components::ImageComponent>(m_InputBlocker);
        image->materialInstance = std::make_shared<render::MaterialInstance>("/engine_assets/materials/editor_inputblocker.sass");
        blockerTransform->layer = -999;
        blockerTransform->needsLayout = true;

        se::ui::components::RectTransformComponent *windowTransform;
        se::ui::components::WindowComponent *windowComp;
        se::ui::components::TitleBarComponent *titleBarComp;
        ecs::Id contentArea;
        ecs::Id titleArea;
        m_Window = ::se::ui::util::CreateWindow(&windowTransform,
                                                &windowComp,
                                                &titleBarComp,
                                                contentArea,
                                                titleArea,
                                                "Project Settings",
                                                editor->GetEditorScene());
        windowTransform->anchors = {0.2f, 0.8f, 0.1f, 0.9f};
        world->AddChild(m_InputBlocker, m_Window);

        auto scrollBox = ::se::ui::util::CreateScrollBox(editor->GetEditorScene());
        world->AddChild(contentArea, scrollBox.scrollBoxEntity);

        m_ScrollBoxContent = world->CreateEntity("Vertical Box");
        auto verticalBox = world->AddComponent<se::ui::components::VerticalBoxComponent>(m_ScrollBoxContent);
        verticalBox->spacing = 5;
        verticalBox->paddingLeft = 2;
        verticalBox->paddingRight = 15;
        world->AddComponent<se::ui::components::WidgetComponent>(m_ScrollBoxContent);
        auto verticalBoxTransform = world->AddComponent<se::ui::components::RectTransformComponent>(m_ScrollBoxContent);
        verticalBoxTransform->anchors = { 0.f, 1.f, 0.f, 0.f };
        world->AddChild(scrollBox.scrollViewEntity, m_ScrollBoxContent);
        // auto db = reflect::SerialiseType<ProjectSettings>(&lol);
        // std::string lol2 = db->ToJson().dump(4);
        // ProjectSettings lol4 = reflect::DeserialiseType<ProjectSettings>(db);
        // int lol3 = 1;

        auto* reflection = geo::CollisionSettings::GetReflection();
        properties::PropertyEditorParams params = {
            .name = "Collision Settings",
            .type = reflection,
            .value = &geo::CollisionSettings::Get(),
            .anchors = {0.f, 1.f, 0.f, 1.f},
            .collapsed = false,
            .withBackground = true,
            .constructTitle = true,
            .contextOptions = {}
        };
        if (auto propEditor = properties::CreatePropertyEditor(params))
        {
            world->AddChild(m_ScrollBoxContent, propEditor->GetWidgetId());
            m_PropertyEditors.push_back(propEditor);
        }
        else
        {
            auto propTextEntity = properties::util::CreateMissingPropertyEditorText(reflection, 0.f, 0);
            world->AddChild(m_ScrollBoxContent, propTextEntity);
        }
    }

    void ProjectSettingsWindow::DestroyUI()
    {
        Application::Get()->GetWorld()->DestroyEntity(m_InputBlocker);
    }

    void ProjectSettingsWindow::CreateCollisionChannels()
    {
    }

    void ProjectSettingsWindow::CreateCollisionMatrix()
    {
    }
}
