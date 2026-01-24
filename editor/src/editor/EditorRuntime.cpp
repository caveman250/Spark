#include "EditorRuntime.h"
#include "engine/render/Renderer.h"
#include "../generated/Classes.generated.h"
#include "../generated/Systems.generated.h"
#include "components/EditorCameraComponent.h"
#include "components/GizmoComponent.h"
#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/asset/shader/Shader.h"
#include "engine/camera/ActiveCameraComponent.h"
#include "engine/ecs/components/MeshComponent.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/input/InputComponent.h"
#include "engine/input/InputUtil.h"
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
        auto world = Application::Get()->GetWorld();

        editor_InitClassReflection();
        editor_InitSystems(world);

        m_StartupManager.RunStartupTasks();

        OnViewportSizeChanged(1280, 720);

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

        //Create camera
        auto camera = world->CreateEntity(GetEditorScene(), "Editor Camera");
        world->AddComponent<components::EditorCameraComponent>(camera);
        auto transform = world->AddComponent<ecs::components::TransformComponent>(camera);
        transform->pos = math::Vec3(7.2f, 5.7f, 12.9f);
        transform->rot = { -0.27f, 3.64f, 0.f };

        CreateEditorPlane();
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

            if (!m_GameMode)
            {
                auto world = Application::Get()->GetWorld();
                if (m_SelectedEntity != se::ecs::s_InvalidEntity && world->HasComponent<ecs::components::TransformComponent>(m_SelectedEntity))
                {
                    if (m_Gizmo == ecs::s_InvalidEntity )
                    {
                        CreateGizmo();
                    }
                    else
                    {
                        SnapGizmoToSelectedEntity();
                    }
                }
                else if (m_Gizmo != ecs::s_InvalidEntity)
                {
                    HideGizmo();
                }
            }

            m_PropertiesWindow->RebuildProperties();
        }

        m_PropertiesWindow->Update();
        m_ViewportWindow->Update();
        m_AssetBrowserWindow->Update();

        if (!m_GameMode)
        {
            auto world = Application::Get()->GetWorld();
            auto activeCamera = world->GetSingletonComponent<camera::ActiveCameraComponent>();
            auto planeModel = world->GetComponent<ecs::components::MeshComponent>(m_Plane);
            if (planeModel->materialInstance)
            {
                planeModel->materialInstance->SetUniform("cameraPos", asset::shader::ast::AstType::Vec3, 1, &activeCamera->pos);
            }
        }
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

    void EditorRuntime::ToggleGameMode()
    {
        auto world = Application::Get()->GetWorld();
        m_GameMode = !m_GameMode;

        if (!m_GameMode)
        {
            world->ReloadAllScenesFromTemp();
            CreateGizmo();
            CreateEditorPlane();
        }
        else
        {
            if (m_Gizmo != ecs::s_InvalidEntity)
            {
                Application::Get()->GetWorld()->DestroyEntity(m_Gizmo);
                m_Gizmo = ecs::s_InvalidEntity;
            }

            if (m_Plane != ecs::s_InvalidEntity)
            {
                Application::Get()->GetWorld()->DestroyEntity(m_Plane);
                m_Plane = ecs::s_InvalidEntity;
            }

            world->SaveAllScenesToTemp();

        }
    }

    void EditorRuntime::CreateGizmo()
    {
        auto world = Application::Get()->GetWorld();

        m_Gizmo = world->CreateEntity(GetEditorScene(), "Translate Gizmo");
        world->AddComponent<ecs::components::TransformComponent>(m_Gizmo);

        auto entityZ = world->CreateEntity(GetEditorScene(), "Translate Gizmo Z");
        auto meshZ = world->AddComponent<ecs::components::MeshComponent>(entityZ);
        meshZ->material = "/engine_assets/materials/gizmo.sass";
        meshZ->model = "/engine_assets/models/translate_handle.sass";
        meshZ->renderLayer = -1;
        auto zTransform = world->AddComponent<ecs::components::TransformComponent>(entityZ);
        zTransform->pos.z = .5f;
        zTransform->scale = .5f;
        auto gizmoZ = world->AddComponent<components::GizmoComponent>(entityZ);
        gizmoZ->color = math::Vec3(0.f, 0.f, 1.f);
        gizmoZ->axis = components::GizmoAxis::Forward;
        gizmoZ->onMove.Subscribe([this, world](math::Vec3 worldPos)
        {
            auto transform = world->GetComponent<ecs::components::TransformComponent>(m_Gizmo);

            worldPos.x = transform->pos.x;
            worldPos.y = transform->pos.y;
            worldPos.z -= .5f;

            UpdateSelectedEntityTranslation(worldPos);
        });
        world->AddChild(m_Gizmo, entityZ);

        auto entityX = world->CreateEntity(GetEditorScene(), "Translate Gizmo X");
        auto meshX = world->AddComponent<ecs::components::MeshComponent>(entityX);
        meshX->material = "/engine_assets/materials/gizmo.sass";
        meshX->model = "/engine_assets/models/translate_handle.sass";
        meshX->renderLayer = -1;
        auto xTransform = world->AddComponent<ecs::components::TransformComponent>(entityX);
        xTransform->rot.y = 90;
        xTransform->pos.x = .5f;
        xTransform->scale = .5f;
        auto gizmoX = world->AddComponent<components::GizmoComponent>(entityX);
        gizmoX->color = math::Vec3(1.f, 0.f, 0.f);
        gizmoX->axis = components::GizmoAxis::Right;
        gizmoX->onMove.Subscribe([this, world](math::Vec3 worldPos)
        {
            auto transform = world->GetComponent<ecs::components::TransformComponent>(m_Gizmo);

            worldPos.y = transform->pos.y;
            worldPos.z = transform->pos.z;
            worldPos.x -= .5f;

            UpdateSelectedEntityTranslation(worldPos);
        });
        world->AddChild(m_Gizmo, entityX);

        auto entityY = world->CreateEntity(GetEditorScene(), "Translate Gizmo Y");
        auto meshY = world->AddComponent<ecs::components::MeshComponent>(entityY);
        meshY->material = "/engine_assets/materials/gizmo.sass";
        meshY->model = "/engine_assets/models/translate_handle.sass";
        meshY->renderLayer = -1;
        auto yTransform = world->AddComponent<ecs::components::TransformComponent>(entityY);
        yTransform->rot.x = 270;
        yTransform->pos.y = .5f;
        yTransform->scale = .5f;
        auto gizmoY = world->AddComponent<components::GizmoComponent>(entityY);
        gizmoY->color = math::Vec3(0.f, 1.f, 0.f);
        gizmoY->axis = components::GizmoAxis::Up;
        gizmoY->onMove.Subscribe([this, world](math::Vec3 worldPos)
        {
            auto transform = world->GetComponent<ecs::components::TransformComponent>(m_Gizmo);

            worldPos.x = transform->pos.x;
            worldPos.y -= .5f;
            worldPos.z = transform->pos.z;

            UpdateSelectedEntityTranslation(worldPos);
        });
        world->AddChild(m_Gizmo, entityY);

        SnapGizmoToSelectedEntity();
    }

    void EditorRuntime::UpdateSelectedEntityTranslation(const math::Vec3& worldPos) const
    {
        auto world = Application::Get()->GetWorld();

        auto selectedEntityTransform = world->GetComponent<ecs::components::TransformComponent>(m_SelectedEntity);
        if (const ecs::Id& parent = world->GetParent(m_SelectedEntity); parent != ecs::s_InvalidEntity)
        {
            auto selectedEntityParentTransform = world->GetComponent<ecs::components::TransformComponent>(parent);
            auto localPos = math::Inverse(selectedEntityParentTransform->worldTransform) * math::Vec4(worldPos, 1);
            selectedEntityTransform->pos = localPos;
        }
        else
        {
            selectedEntityTransform->pos = worldPos;
        }

        SnapGizmoToSelectedEntity();
    }

    void EditorRuntime::SnapGizmoToSelectedEntity() const
    {
        if (m_SelectedEntity == ecs::s_InvalidEntity)
        {
            HideGizmo();
            return;
        }

        const auto world = Application::Get()->GetWorld();

        const auto selectedEntityTransform = world->GetComponent<ecs::components::TransformComponent>(m_SelectedEntity);
        const auto gizmoTransform = world->GetComponent<ecs::components::TransformComponent>(m_Gizmo);
        gizmoTransform->pos = selectedEntityTransform->worldTransform[3];
    }

    void EditorRuntime::HideGizmo() const
    {
        const auto world = Application::Get()->GetWorld();

        auto transform = world->GetComponent<ecs::components::TransformComponent>(m_Gizmo);
        transform->pos = math::Vec3(std::numeric_limits<float>::max());
    }

    void EditorRuntime::CreateEditorPlane()
    {
        auto world = Application::Get()->GetWorld();

        m_Plane = world->CreateEntity(GetEditorScene(), "Editor Plane");
        auto planeTransform = world->AddComponent<ecs::components::TransformComponent>(m_Plane);
        planeTransform->rot.x = -90;
        planeTransform->scale = 100;
        auto planeModel = world->AddComponent<ecs::components::MeshComponent>(m_Plane);
        planeModel->model = "/engine_assets/models/plane.sass";
        planeModel->material = "/engine_assets/materials/editor_plane.sass";
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
