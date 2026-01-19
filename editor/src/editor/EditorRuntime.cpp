#include "EditorRuntime.h"
#include "engine/render/Renderer.h"
#include "../generated/Classes.generated.h"
#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/asset/shader/Shader.h"
#include "engine/io/VFS.h"
#include "engine/render/Material.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/SplitViewComponent.h"
#include "engine/ui/components/WindowComponent.h"

namespace se::editor
{
    ecs::Id AddSplitView(const ecs::Id& scene,
        const ecs::Id& window1,
        const ecs::Id& window2,
        const se::ui::components::SplitViewDirection dir,
        const float sliderVal)
    {
        auto world = Application::Get()->GetWorld();
        auto assetManager = asset::AssetManager::Get();

        ecs::Id splitView = world->CreateEntity(scene, "SplitView");
        auto splitViewComp = world->AddComponent<se::ui::components::SplitViewComponent>(splitView);
        splitViewComp->dir = dir;
        splitViewComp->sliderPos = sliderVal;
        world->AddComponent<se::ui::components::WidgetComponent>(splitView);
        world->AddComponent<se::ui::components::MouseInputComponent>(splitView);
        auto keyInput = world->AddComponent<se::ui::components::KeyInputComponent>(splitView);
        keyInput->keyMask = input::Key::Unknown;

        auto rect = world->AddComponent<se::ui::components::RectTransformComponent>(splitView);
        rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        rect->needsLayout = true;

        world->AddChild(splitView, window1);
        splitViewComp->entity1 = window1;
        if (world->HasComponent<se::ui::components::WindowComponent>(window1))
        {
            auto window = world->GetComponent<se::ui::components::WindowComponent>(window1);
            window->docked = true;
        }

        splitViewComp->sliderEntity = world->CreateEntity(scene, "Slider");
        world->AddComponent<se::ui::components::RectTransformComponent>(splitViewComp->sliderEntity);
        world->AddComponent<se::ui::components::WidgetComponent>(splitViewComp->sliderEntity);
        auto bgMaterial = assetManager->GetAsset<render::Material>("/engine_assets/materials/editor_splitview_slider.sass");
        auto sliderImage = world->AddComponent<se::ui::components::ImageComponent>(splitViewComp->sliderEntity);
        sliderImage->materialInstance = se::render::MaterialInstance::CreateMaterialInstance(bgMaterial);
        world->AddChild(splitView, splitViewComp->sliderEntity);

        world->AddChild(splitView, window2);
        splitViewComp->entity2 = window2;
        if (world->HasComponent<se::ui::components::WindowComponent>(window2))
        {
            auto window = world->GetComponent<se::ui::components::WindowComponent>(window2);
            window->docked = true;
        }

        return splitView;
    }
    void EditorRuntime::Init()
    {
        editor_InitClassReflection();

        m_StartupManager.RunStartupTasks();

        OnViewportSizeChanged(1280, 720);

        auto world = Application::Get()->GetWorld();
        m_EditorScene = world->CreateScene("Editor");

        m_OutlineWindow = new ui::OutlineWindow(this);
        m_OutlineWindow->ConstructUI();

        m_PropertiesWindow = new ui::PropertiesWindow(this);
        m_PropertiesWindow->ConstructUI();

        m_ViewportWindow = new ui::ViewportWindow(this, [this](int x, int y)
        {
            auto window = Application::Get()->GetWindow();
            const float reciprocal = 1.f / window->GetContentScale();
            OnViewportSizeChanged(static_cast<int>(x * reciprocal), static_cast<int>(y * reciprocal));
        });
        m_ViewportWindow->ConstructUI();

        m_AssetBrowserWindow = new ui::AssetBrowserWindow(this);
        m_AssetBrowserWindow->ConstructUI();

        ecs::Id splitView1 = AddSplitView(m_EditorScene,
            m_OutlineWindow->GetWindow(),
            m_ViewportWindow->GetWindow(),
            se::ui::components::SplitViewDirection::Horizontal,
            0.3f);

         ecs::Id splitView2 = AddSplitView(m_EditorScene,
             splitView1,
             m_AssetBrowserWindow->GetWindow(),
             se::ui::components::SplitViewDirection::Vertical,
             0.6f);

         AddSplitView(m_EditorScene,
             splitView2,
             m_PropertiesWindow->GetWindow(),
             se::ui::components::SplitViewDirection::Horizontal,
             0.7f);
    }

    void EditorRuntime::Update()
    {
        if (m_SelectedEntity != m_LastSelectedEntity ||
            m_SelectedAsset != m_LastSelectedAsset ||
            m_SelectedSingletonComp != m_LastSelectedSingletonComp)
        {
            m_LastSelectedEntity = m_SelectedEntity;
            m_LastSelectedAsset = m_SelectedAsset;
            m_LastSelectedSingletonComp = m_SelectedSingletonComp;

            m_PropertiesWindow->RebuildProperties();
        }

        m_PropertiesWindow->Update();
        m_ViewportWindow->Update();
    }

    void EditorRuntime::Render()
    {
        auto renderer = render::Renderer::Get<render::Renderer>();
        m_OffscreenRenderGroup = renderer->AllocRenderGroup(0);
        renderer->SetFrameBuffer(m_OffscreenRenderGroup, m_FrameBuffer);

        renderer->Submit<render::commands::Clear>(m_OffscreenRenderGroup, true, true);
    }

    void EditorRuntime::Shutdown()
    {
    }

    const ecs::Id& EditorRuntime::GetEditorScene() const
    {
        return m_EditorScene;
    }

    const ecs::Id& EditorRuntime::GetSelectedEntity() const
    {
        return m_SelectedEntity;
    }

    void EditorRuntime::SelectEntity(const ecs::Id& id)
    {
        m_SelectedEntity = id;
        m_SelectedSingletonComp = nullptr;
        m_SelectedAsset = nullptr;
    }

    reflect::ObjectBase* EditorRuntime::GetSelectedSingletonComponent() const
    {
        return m_SelectedSingletonComp;
    }

    void EditorRuntime::SelectSingletonComponent(reflect::ObjectBase* comp)
    {
        m_SelectedEntity = ecs::s_InvalidEntity;
        m_SelectedSingletonComp = comp;
        m_SelectedAsset = nullptr;
    }

    void EditorRuntime::OnEntitiesChanged() const
    {
        m_OutlineWindow->RebuildOutline();
    }

    void EditorRuntime::OnViewportSizeChanged(int x, int y)
    {
        if (m_FrameBuffer)
        {
            m_FrameBuffer.reset();
        }
        m_FrameBuffer = render::FrameBuffer::CreateFrameBuffer({ x, y });
    }

    const std::shared_ptr<asset::Asset>& EditorRuntime::GetSelectedAsset() const
    {
        return m_SelectedAsset;
    }

    void EditorRuntime::SelectAsset(const std::shared_ptr<asset::Asset>& asset)
    {
        m_SelectedEntity = ecs::s_InvalidEntity;
        m_SelectedSingletonComp = nullptr;
        m_SelectedAsset = asset;
    }
}
