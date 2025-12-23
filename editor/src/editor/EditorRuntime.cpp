#include "EditorRuntime.h"
#include "engine/render/Renderer.h"
#include "../generated/Classes.generated.h"
#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/asset/shader/Shader.h"
#include "engine/render/Material.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/MouseInputComponent.h"
#include "engine/ui/components/SplitViewComponent.h"
#include "engine/ui/components/WindowComponent.h"

namespace se::editor
{
    ecs::Id AddSplitView(const ecs::Id& window1,
        const ecs::Id& window2,
        const se::ui::components::SplitViewDirection dir,
        const float sliderVal)
    {
        auto world = Application::Get()->GetWorld();
        ecs::Id splitView = world->CreateEntity("SplitView", true);
        auto splitViewComp = world->AddComponent<se::ui::components::SplitViewComponent>(splitView);
        splitViewComp->dir = dir;
        splitViewComp->sliderPos = sliderVal;
        world->AddComponent<se::ui::components::WidgetComponent>(splitView);
        auto mouseInput = world->AddComponent<se::ui::components::MouseInputComponent>(splitView);
        mouseInput->buttonMask = 0;
        auto keyInput = world->AddComponent<se::ui::components::KeyInputComponent>(splitView);
        keyInput->keyMask = input::Key::Unknown;

        auto rect = world->AddComponent<se::ui::components::RectTransformComponent>(splitView);
        rect->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        rect->needsLayout = true;
        rect->overridesChildSizes = true;

        world->AddChild(splitView, window1);
        splitViewComp->entity1 = window1;
        if (world->HasComponent<se::ui::components::WindowComponent>(window1))
        {
            auto window = world->GetComponent<se::ui::components::WindowComponent>(window1);
            window->docked = true;
        }

        splitViewComp->sliderEntity = world->CreateEntity("Slider", true);
        world->AddComponent<se::ui::components::RectTransformComponent>(splitViewComp->sliderEntity);
        world->AddComponent<se::ui::components::WidgetComponent>(splitViewComp->sliderEntity);
        static std::shared_ptr<render::Material> bgMaterial = nullptr;
        if (!bgMaterial)
        {
            auto assetManager = asset::AssetManager::Get();
            auto vert = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/ui.sass");
            auto frag = assetManager->GetAsset<asset::Shader>("/engine_assets/shaders/flat_color.sass");
            bgMaterial = render::Material::CreateMaterial({vert}, {frag});
            bgMaterial->GetShaderSettings().SetSetting("color_setting", math::Vec3(.15f, .15f, .15f));
        }
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

        m_FrameBuffer = render::FrameBuffer::CreateFrameBuffer({ 1280, 720 });

        m_OutlineWindow = new ui::OutlineWindow(this);
        m_OutlineWindow->ConstructUI();

        m_PropertiesWindow = new ui::PropertiesWindow(this);
        m_PropertiesWindow->ConstructUI();

        m_ViewportWindow = new ui::ViewportWindow(this);
        m_ViewportWindow->ConstructUI();

        m_AssetBrowserWindow = new ui::AssetBrowserWindow(this);
        m_AssetBrowserWindow->ConstructUI();

        ecs::Id splitView1 = AddSplitView(m_OutlineWindow->GetWindow(),
            m_ViewportWindow->GetWindow(),
            se::ui::components::SplitViewDirection::Horizontal,
            0.3f);

         ecs::Id splitView2 = AddSplitView(splitView1,
             m_AssetBrowserWindow->GetWindow(),
             se::ui::components::SplitViewDirection::Vertical,
             0.6f);

         AddSplitView(splitView2,
             m_PropertiesWindow->GetWindow(),
             se::ui::components::SplitViewDirection::Horizontal,
             0.7f);
    }

    void EditorRuntime::Update()
    {
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

    ecs::Id EditorRuntime::GetSelectedEntity() const
    {
        return m_SelectedEntity;
    }

    void EditorRuntime::SelectEntity(const ecs::Id& id)
    {
        m_SelectedEntity = id;
        m_SelectedSingletonComp = nullptr;
        m_SelectedAsset = nullptr;
        m_PropertiesWindow->RebuildProperties();
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
        m_PropertiesWindow->RebuildProperties();
    }

    void EditorRuntime::OnEntitiesChanged() const
    {
        m_OutlineWindow->RebuildOutline();
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
        m_PropertiesWindow->RebuildProperties();
    }
}
