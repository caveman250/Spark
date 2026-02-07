#include "ViewportWindow.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/asset/AssetManager.h"
#include "engine/Application.h"
#include "engine/ui/components/WindowComponent.h"
#include "engine/ui/components/TitleBarComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/util/WindowUtil.h"
#include "engine/render/MaterialInstance.h"
#include "engine/render/FrameBuffer.h"
#include "engine/render/render_fwd.h"
#include "engine/ui/util/ComboBoxUtil.h"

namespace se::editor::ui
{
    void ViewportWindow::Update()
    {
        ToolWindow::Update();
        if (!m_Valid)
        {
            return;
        }

        auto app = Application::Get();
        auto editor = app->GetEditorRuntime();
        auto world = app->GetWorld();
        se::ui::components::RectTransformComponent* viewportRect = world->GetComponent<se::ui::components::RectTransformComponent>(m_Viewport);

        // awaiting layout.
        if (viewportRect->rect.size.x < 0 || viewportRect->rect.size.y < 0)
        {
            return;
        }

        if (viewportRect->rect != m_ViewportRect)
        {
            m_OnViewportSizeChanged(viewportRect->rect.size.x, viewportRect->rect.size.y);
            const auto& viewportTexture = editor->GetFrameBuffer()->GetColorTexture();
            auto imageComp = world->GetComponent<se::ui::components::ImageComponent>(m_Viewport);
            imageComp->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &viewportTexture);
        }
        m_ViewportRect = viewportRect->rect;

        if (m_ShouldToggleGameMode)
        {
            editor->ToggleGameMode();
            UpdatePlayButtonTexture();
            m_ShouldToggleGameMode = false;
        }
    }

    void ViewportWindow::ConstructUI()
    {
        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();

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
                                                "Viewport",
                                                editor->GetEditorScene());
        windowTransform->anchors = {0.f, 0.f, 0.f, 0.f};
        windowTransform->minX = 310;
        windowTransform->maxX = 850;
        windowTransform->minY = 20;
        windowTransform->maxY = 380;

        m_Viewport = world->CreateEntity(editor->GetEditorScene(), "Viewport Image");
        auto imageTransform = world->AddComponent<se::ui::components::RectTransformComponent>(m_Viewport);
        imageTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        world->AddComponent<se::ui::components::WidgetComponent>(m_Viewport);
        auto imageComp = world->AddComponent<se::ui::components::ImageComponent>(m_Viewport);
        static std::shared_ptr<render::Material> material = nullptr;
        if (!material)
        {
            material = std::make_shared<render::Material>(
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/ui.sass") },
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/framebuffer_texture.sass") });
#if OPENGL_RENDERER
            material->GetShaderSettings().SetSetting("ymod", -1.f);
#else
            material->GetShaderSettings().SetSetting("ymod", 1.f);
#endif
        }

        imageComp->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        const auto& viewportTexture = app->GetEditorRuntime()->GetFrameBuffer()->GetColorTexture();
        imageComp->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &viewportTexture);
        world->AddChild(contentArea, m_Viewport);
        m_Valid = true;

        auto playPauseBg = world->CreateEntity(editor->GetEditorScene(), "BG");
        auto playPauseBgRect = world->AddComponent<se::ui::components::RectTransformComponent>(playPauseBg);
        playPauseBgRect->anchors = { .left = 0.5f, .right = 0.5f, .top = 0.f, .bottom = 0.f };
        playPauseBgRect->minX = -52;
        playPauseBgRect->maxX = 52;
        playPauseBgRect->minY = 4;
        playPauseBgRect->maxY = 28;
        auto playPauseBgImage = world->AddComponent<se::ui::components::ImageComponent>(playPauseBg);
        auto bgMaterial = asset::AssetManager::Get()->GetAsset<render::Material>("/engine_assets/materials/ui_alpha_texture.sass");
        playPauseBgImage->materialInstance = render::MaterialInstance::CreateMaterialInstance(bgMaterial);
        asset::AssetReference<asset::Texture> bgImage = "/engine_assets/textures/editor_play_pause_bg.sass";
        playPauseBgImage->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2DReference, 1, &bgImage);
        world->AddChild(titleArea, playPauseBg);

        m_PlayButton = world->CreateEntity(editor->GetEditorScene(), "Editor Play Button");
        auto playButtonRect = world->AddComponent<se::ui::components::RectTransformComponent>(m_PlayButton);
        playButtonRect->anchors = { .left = 0.5f, .right = 0.5f, .top = 0.f, .bottom = 0.f };
        playButtonRect->minX = -25;
        playButtonRect->maxX = -5;
        playButtonRect->minY = 2;
        playButtonRect->maxY = 22;
        auto playButton = world->AddComponent<se::ui::components::ButtonComponent>(m_PlayButton);
        playButton->image = "/engine_assets/textures/editor_play.sass";
        playButton->pressedImage = "/engine_assets/textures/editor_play_pressed.sass";
        playButton->hoveredImage = "/engine_assets/textures/editor_play_hovered.sass";
        playButton->onReleased.Subscribe([this, world](input::MouseButton)
        {
            if (world->Paused())
            {
                world->TogglePause();
                UpdatePauseButtonTexture();
            }

            m_ShouldToggleGameMode = true;
        });
        world->AddChild(playPauseBg, m_PlayButton);

        m_PauseButton = world->CreateEntity(editor->GetEditorScene(), "Editor Play Button");
        auto pauseButtonRect = world->AddComponent<se::ui::components::RectTransformComponent>(m_PauseButton);
        pauseButtonRect->anchors = { .left = 0.5f, .right = 0.5f, .top = 0.f, .bottom = 0.f };
        pauseButtonRect->minX = 5;
        pauseButtonRect->maxX = 25;
        pauseButtonRect->minY = 2;
        pauseButtonRect->maxY = 22;
        auto pauseButton = world->AddComponent<se::ui::components::ButtonComponent>(m_PauseButton);
        pauseButton->image = "/engine_assets/textures/editor_pause.sass";
        pauseButton->pressedImage = "/engine_assets/textures/editor_pause_pressed.sass";
        pauseButton->hoveredImage = "/engine_assets/textures/editor_pause_hovered.sass";
        pauseButton->onReleased.Subscribe([this, world](input::MouseButton)
        {
            auto editor = Application::Get()->GetEditorRuntime();
            if (!world->Paused() && !editor->InGameMode())
            {
                return;
            }

            world->TogglePause();
            UpdatePauseButtonTexture();
        });
        world->AddChild(playPauseBg, m_PauseButton);

        ecs::Id titleBarTextEntity = world->CreateEntity(editor->GetEditorScene(), "TitleBarText");
        auto titleBarText = world->AddComponent<se::ui::components::TextComponent>(titleBarTextEntity);
        titleBarText->font = "/engine_assets/fonts/Arial.sass";
        titleBarText->fontSize = 14;
        titleBarText->text = "Show UI";
        auto titleBarTextTransform = world->AddComponent<se::ui::components::RectTransformComponent>(titleBarTextEntity);
        titleBarTextTransform->minX = 82;
        titleBarTextTransform->minY = 8;
        titleBarTextTransform->maxY = 26;
        titleBarTextTransform->anchors = { 1.f, 1.f, 0.f, 0.f };
        world->AddComponent<se::ui::components::WidgetComponent>(titleBarTextEntity);
        world->AddChild(titleArea, titleBarTextEntity);

        m_UIVisibleCheckBox = world->CreateEntity(editor->GetEditorScene(), "Border");
        auto button = world->AddComponent<se::ui::components::ButtonComponent>(m_UIVisibleCheckBox);
        static asset::AssetReference<asset::Texture> checkedTexture = "/engine_assets/textures/checkbox_checked.sass";
        static asset::AssetReference<asset::Texture> unCheckedTexture = "/engine_assets/textures/checkbox_unchecked.sass";
        button->image = m_UIVisible ? checkedTexture : unCheckedTexture;
        button->pressedImage = m_UIVisible ? checkedTexture : unCheckedTexture;
        button->hoveredImage = m_UIVisible ? checkedTexture : unCheckedTexture;
        button->onReleased.Subscribe([this, world](input::MouseButton)
        {
            m_UIVisible = !m_UIVisible;
            world->SetUIVisibility(m_UIVisible);

            auto button = world->GetComponent<se::ui::components::ButtonComponent>(m_UIVisibleCheckBox);
            button->image = m_UIVisible ? checkedTexture : unCheckedTexture;
            button->pressedImage = m_UIVisible ? checkedTexture : unCheckedTexture;
            button->hoveredImage = m_UIVisible ? checkedTexture : unCheckedTexture;
        });
        auto innerTransform = world->AddComponent<se::ui::components::RectTransformComponent>(m_UIVisibleCheckBox);
        innerTransform->anchors = { .left = 1.f, .right = 1.f, .top = 0.f, .bottom = 0.f };
        innerTransform->minY = 6;
        innerTransform->minX = 24;
        innerTransform->maxY = 24;
        innerTransform->maxX = 6;
        world->AddChild(titleArea, m_UIVisibleCheckBox);
    }

    void ViewportWindow::DestroyUI()
    {
        m_Valid = false;
    }

    void ViewportWindow::UpdatePlayButtonTexture() const
    {
        auto app = Application::Get();
        auto world = app->GetWorld();
        auto editor = app->GetEditorRuntime();
        auto playButton = world->GetComponent<se::ui::components::ButtonComponent>(m_PlayButton);
        if (!editor->InGameMode())
        {
            playButton->image = "/engine_assets/textures/editor_play.sass";
            playButton->pressedImage = "/engine_assets/textures/editor_play_pressed.sass";
            playButton->hoveredImage = "/engine_assets/textures/editor_play_hovered.sass";
        }
        else
        {
            playButton->image = "/engine_assets/textures/editor_stop.sass";
            playButton->pressedImage = "/engine_assets/textures/editor_stop_pressed.sass";
            playButton->hoveredImage = "/engine_assets/textures/editor_stop_hovered.sass";
        }
    }

    void ViewportWindow::UpdatePauseButtonTexture() const
    {
        auto world = Application::Get()->GetWorld();
        if (world->Paused())
        {
            auto pauseButton = world->GetComponent<se::ui::components::ButtonComponent>(m_PauseButton);
            pauseButton->image = "/engine_assets/textures/editor_pause_active.sass";
            pauseButton->pressedImage = "/engine_assets/textures/editor_pause_active_pressed.sass";
            pauseButton->hoveredImage = "/engine_assets/textures/editor_pause_active_hovered.sass";
        }
        else
        {
            auto pauseButton = world->GetComponent<se::ui::components::ButtonComponent>(m_PauseButton);
            pauseButton->image = "/engine_assets/textures/editor_pause.sass";
            pauseButton->pressedImage = "/engine_assets/textures/editor_pause_pressed.sass";
            pauseButton->hoveredImage = "/engine_assets/textures/editor_pause_hovered.sass";
        }
    }
}
