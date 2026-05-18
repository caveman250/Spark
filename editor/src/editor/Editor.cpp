#include "Editor.h"

#include "EditorShortcutsManager.h"
#include "../generated/Classes.generated.h"
#include "../generated/Systems.generated.h"
#include "components/EditorCameraComponent.h"
#include "engine/Application.h"
#include "engine/asset/AssetManager.h"
#include "engine/asset/builder/AssetBuilder.h"
#include "engine/asset/meta/MetaDataManager.h"
#include "engine/asset/util/AssetUtil.h"
#include "engine/camera/ActiveCameraComponent.h"
#include "engine/ecs/Prefab.h"
#include "engine/ecs/components/MeshComponent.h"
#include "engine/ecs/components/TransformComponent.h"
#include "engine/geo/Plane.h"
#include "engine/geo/util/CollisionUtil.h"
#include "engine/input/InputComponent.h"
#include "engine/io/VFS.h"
#include "engine/render/Renderer.h"
#include "engine/render/singleton_components/MeshRenderComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/ui/components/KeyInputComponent.h"
#include "engine/ui/components/SplitViewComponent.h"
#include "engine/ui/util/SplitViewUtil.h"
#include "util/ViewportUtil.h"

namespace se::editor
{
    void Editor::Init()
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

        m_AssetBrowserWindow = new ui::asset_browser::AssetBrowserWindow(this);
        m_AssetBrowserWindow->ConstructUI();

        ecs::Id splitView1 = se::ui::util::AddSplitView(m_EditorScene,
            m_OutlineWindow->GetWindow(),
            m_ViewportWindow->GetWindow(),
            se::ui::components::SplitViewDirection::Horizontal,
            0.3f);

        ecs::Id splitView2 = se::ui::util::AddSplitView(m_EditorScene,
            splitView1,
            m_AssetBrowserWindow->GetWindow(),
            se::ui::components::SplitViewDirection::Vertical,
            0.6f);

        se::ui::util::AddSplitView(m_EditorScene,
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

        EditorShortcutsManager::RegisterShortcuts();
    }

    void Editor::Update()
    {
        auto* meshRenderComp = Application::Get()->GetWorld()->GetSingletonComponent<render::singleton_components::MeshRenderComponent>();
        meshRenderComp->invalidatedMeshAssets.clear();

        if (m_ForceSelection ||
            m_SelectedEntity != m_LastSelectedEntity ||
            m_SelectedAsset != m_LastSelectedAsset ||
            m_SelectedSingletonComp != m_LastSelectedSingletonComp)
        {
            m_LastSelectedEntity = m_SelectedEntity;
            m_LastSelectedAsset = m_SelectedAsset;
            m_LastSelectedSingletonComp = m_SelectedSingletonComp;
            m_ForceSelection = false;

            if (!m_GameMode)
            {
                m_GizmoManager.OnSelectEntity(m_SelectedEntity);
            }

            m_PropertiesWindow->RebuildProperties();
        }

        m_PropertiesWindow->Update();
        m_ViewportWindow->Update();
        m_AssetBrowserWindow->Update();
        m_OutlineWindow->Update();

        auto world = Application::Get()->GetWorld();
        if (!m_GameMode)
        {
            auto activeCamera = world->GetSingletonComponent<camera::ActiveCameraComponent>();
            auto planeModel = world->GetComponent<ecs::components::MeshComponent>(m_Plane);
            if (planeModel->materialInstance)
            {
                planeModel->materialInstance->SetUniform("cameraPos", 1, &activeCamera->pos);
            }
        }

        const auto* shortcutsComp = world->GetSingletonComponent<singleton_components::EditorShortcutsComponent>();
        for (const auto& func : shortcutsComp->pendingShortcuts)
        {
            func();
        }
    }

    void Editor::Render()
    {
        auto* renderer = render::Renderer::Get<render::Renderer>();
        m_OffscreenRenderGroup = renderer->AllocRenderGroup(0);
        renderer->SetFrameBuffer(m_OffscreenRenderGroup, m_FrameBuffer);

        renderer->Submit<render::commands::Clear>(m_OffscreenRenderGroup, true, true);
    }

    void Editor::Shutdown()
    {
    }

    const ecs::Id& Editor::GetEditorScene() const
    {
        return m_EditorScene;
    }

    const ecs::Id& Editor::GetLoadedScene() const
    {
        return m_LoadedScene;
    }

    void Editor::LoadScene(const std::string& path)
    {
        auto world = Application::Get()->GetWorld();
        if (m_LoadedScene != ecs::InvalidEntity)
        {
            world->UnloadScene(m_LoadedScene);
        }
        m_LoadedScene = world->LoadScene(path);
        m_ScenePath = path;
    }

    const ecs::Id& Editor::GetSelectedEntity() const
    {
        return m_SelectedEntity;
    }

    void Editor::SelectEntity(const ecs::Id& id, bool force)
    {
        m_SelectedEntity = id;
        m_SelectedSingletonComp = nullptr;
        m_SelectedAsset = nullptr;
        m_ForceSelection = force;
    }

    reflect::ObjectBase* Editor::GetSelectedSingletonComponent() const
    {
        return m_SelectedSingletonComp;
    }

    void Editor::SelectSingletonComponent(reflect::ObjectBase* comp, bool force)
    {
        m_SelectedEntity = ecs::InvalidEntity;
        m_SelectedSingletonComp = comp;
        m_SelectedAsset = nullptr;
        m_ForceSelection = force;
    }

    void Editor::OnEntitiesChanged() const
    {
        m_OutlineWindow->RebuildOutline();
    }

    void Editor::Copy()
    {
        if (m_SelectedEntity != ecs::InvalidEntity)
        {
            m_EntityToCopy = m_SelectedEntity;
        }
    }

    void Editor::Cut()
    {
        auto world = Application::Get()->GetWorld();
        m_CutEntity = world->CreatePrefabFromEntity(m_SelectedEntity);
        world->DestroyEntity(m_SelectedEntity);
        m_SelectedEntity = ecs::InvalidEntity;
        m_EntityToCopy = ecs::InvalidEntity;
    }

    void Editor::Paste()
    {
        auto* world = Application::Get()->GetWorld();

        ecs::Id newEntity = {};
        if (m_EntityToCopy != ecs::InvalidEntity)
        {
            newEntity = world->DuplicateEntity(m_EntityToCopy);
        }
        else if (!m_CutEntity.m_Entities.empty())
        {
            newEntity = world->InstantiatePrefab(GetLoadedScene(), m_CutEntity);
        }

        if (newEntity != ecs::InvalidEntity)
        {
            if (auto* transform = world->GetComponent<ecs::components::TransformComponent>(newEntity))
            {
                auto ray = util::GetEditorMouseRay(world->GetSingletonComponent<input::InputComponent>(),
                                                    world->GetSingletonComponent<camera::ActiveCameraComponent>());
                geo::Plane plane = {
                    .normal = { 0.f, 1.f, 0.f },
                    .center = transform->pos
                };
                const auto hit = geo::util::RayCastPlane(ray, plane);
                if (hit.has_value())
                {
                    transform->pos = hit.value().intersectionPoint;
                }
            }

            SelectEntity(newEntity);
        }
    }

    bool Editor::HasValidCopySelection() const
    {
        return m_SelectedEntity != ecs::InvalidEntity ||
                m_SelectedAsset;
    }

    bool Editor::HasValidPasteTarget() const
    {
        return m_EntityToCopy != ecs::InvalidEntity || !m_CutEntity.m_Entities.empty();
    }

    void Editor::OnViewportSizeChanged(int x, int y)
    {
        if (m_FrameBuffer)
        {
            m_FrameBuffer.reset();
        }
        m_FrameBuffer = render::FrameBuffer::CreateFrameBuffer({ x, y });
    }

    void Editor::ToggleGameMode()
    {
        auto world = Application::Get()->GetWorld();
        m_GameMode = !m_GameMode;

        if (!m_GameMode)
        {
            m_LoadedScene = world->ReloadSceneFromTemp(m_LoadedScene);
            m_GizmoManager.CreateGizmo();
            CreateEditorPlane();
        }
        else
        {
            m_GizmoManager.DestroyGizmo();

            if (m_Plane != ecs::InvalidEntity)
            {
                Application::Get()->GetWorld()->DestroyEntity(m_Plane);
                m_Plane = ecs::InvalidEntity;
            }

            world->SaveSceneToTemp(m_LoadedScene);
        }
    }

    void Editor::SaveAll()
    {
        if (m_GameMode)
        {
            return;
        }

        if (m_LoadedScene != ecs::InvalidEntity)
        {
            SaveScene();
        }

        if (m_SelectedAsset)
        {
            SaveAsset(m_SelectedAsset);
        }
    }

    std::string Editor::DuplicateAsset(const std::shared_ptr<asset::Asset>& asset)
    {
        auto type = asset->GetReflectType();
        auto& vfs = io::VFS::Get();
        auto metaManager = asset::meta::MetaManager::Get();

        auto db = asset::binary::Database::Create(false);
        db->SetRootStruct(db->GetOrCreateStruct(type->GetTypeName(nullptr), type->GetStructLayout(nullptr)));
        auto root = db->GetRoot();
        type->Serialize(asset.get(), root, {});

        auto sourcePathExtensionIt = asset->m_SourcePath.find(".");
        std::string newSourcePath = asset->m_SourcePath;
        newSourcePath.replace(sourcePathExtensionIt, 1, "_1.");
        root.Set("m_SourcePath", newSourcePath);

        auto metaPath = metaManager->GetMetaPath(asset->m_Path);

        int index = 1;
        while (vfs.Exists(newSourcePath))
        {
            index++;
            newSourcePath.replace(sourcePathExtensionIt, 2, std::format("_{}", index));
        }

        if (asset->UsesMetaData())
        {
            auto newMetaPath = metaManager->GetMetaPath(newSourcePath);
            vfs.Copy(metaPath, newMetaPath);
        }

        std::string newPath = asset::util::GetAssetBuiltPath(newSourcePath);
        db->Save(newPath);
        vfs.Copy(asset->m_SourcePath, newSourcePath);

        return newPath;
    }

    void Editor::SaveAsset(const std::shared_ptr<asset::Asset>& asset)
    {
        auto metaManager = asset::meta::MetaManager::Get();
        if (asset->UsesMetaData())
        {
            auto metaPath = metaManager->GetMetaPath(asset->m_Path);
            auto meta = metaManager->GetOrCreateMetaDataForAsset(asset.get());

            auto type = meta->GetReflectType();
            auto db = asset::binary::Database::Create(false);
            db->SetRootStruct(db->GetOrCreateStruct(type->GetTypeName(nullptr), type->GetStructLayout(nullptr)));
            auto root = db->GetRoot();
            type->Serialize(meta.get(), root, {});

            io::VFS::Get().WriteText(metaPath, db->ToJson().dump(4));

            auto* bp = asset::builder::AssetBuilder::GetBlueprintForAsset(asset->m_SourcePath);
            for (const auto& builtAsset : bp->BuildAsset(asset->m_SourcePath, asset->m_Path))
            {
                if (!builtAsset.fileNameSuffix.empty())
                {
                    auto outputPath = asset->m_Path;
                    auto extensionIt = outputPath.find_last_of(".");
                    outputPath.insert(extensionIt, builtAsset.fileNameSuffix);
                    builtAsset.db->Save(outputPath);
                }
                else
                {
                    builtAsset.db->Save(asset->m_Path);
                }
            }
            asset::AssetManager::Get()->ForceReloadAsset(asset->m_Path, asset->GetReflectType());
        }
        else
        {
            auto type = asset->GetReflectType();
            auto db = asset::binary::Database::Create(false);
            db->SetRootStruct(db->GetOrCreateStruct(type->GetTypeName(nullptr), type->GetStructLayout(nullptr)));
            auto root = db->GetRoot();
            type->Serialize(asset.get(), root, {});

            db->Save(asset->m_Path);
            io::VFS::Get().WriteText(asset->m_SourcePath, db->ToJson().dump(4));
        }
    }

    void Editor::DeleteAsset(const std::shared_ptr<asset::Asset>& asset)
    {
        auto& vfs = io::VFS::Get();
        if (asset->UsesMetaData())
        {
            auto metaManager = asset::meta::MetaManager::Get();
            auto metaPath = metaManager->GetMetaPath(asset->m_Path);
            vfs.Delete(metaPath);
        }
        vfs.Delete(asset->m_Path);
        vfs.Delete(asset->m_SourcePath);
    }

    void Editor::RenameAsset(const std::shared_ptr<asset::Asset>& asset,
        const std::string& newPath)
    {
        auto metaManager = asset::meta::MetaManager::Get();
        auto& vfs = io::VFS::Get();

        std::string oldPath = asset->m_Path;
        std::string metaPath = metaManager->GetMetaPath(asset->m_Path);
        std::string oldSourcePath = asset->m_SourcePath;
        std::string newSourcePath = asset::util::GetSourcePath(newPath, asset->GetSourceFileExtension());

        asset->m_Path = newPath;
        asset->m_SourcePath = newSourcePath;
        auto type = asset->GetReflectType();
        auto db = asset::binary::Database::Create(false);
        db->SetRootStruct(db->GetOrCreateStruct(type->GetTypeName(nullptr), type->GetStructLayout(nullptr)));
        auto root = db->GetRoot();
        type->Serialize(asset.get(), root, {});
        db->Save(asset->m_Path);
        vfs.Delete(oldPath);

        vfs.Rename(oldSourcePath, newSourcePath);

        if (asset->UsesMetaData())
        {
            std::string newMetaPath = metaManager->GetMetaPath(asset->m_Path);
            vfs.Rename(metaPath, newMetaPath);
        }

        if (m_ScenePath == oldPath)
        {
            m_ScenePath = newPath;
        }
    }

    void Editor::DeSelectAll()
    {
        m_SelectedEntity = ecs::InvalidEntity;
        m_SelectedSingletonComp = nullptr;
        m_SelectedAsset = nullptr;
    }

    void Editor::SaveScene()
    {
        if (SPARK_VERIFY(m_LoadedScene))
        {
            auto world = Application::Get()->GetWorld();
            world->SaveScene(m_LoadedScene, m_ScenePath, true);
            world->SaveScene(m_LoadedScene, asset::util::GetSourcePath(m_ScenePath, ".json"), false);
        }
    }

    void Editor::CreateEditorPlane()
    {
        auto world = Application::Get()->GetWorld();

        m_Plane = world->CreateEntity(GetEditorScene(), "Editor Plane");
        auto planeTransform = world->AddComponent<ecs::components::TransformComponent>(m_Plane);
        planeTransform->rot.x = -90;
        planeTransform->scale = 100;
        auto planeModel = world->AddComponent<ecs::components::MeshComponent>(m_Plane);
        planeModel->model = "/engine_assets/models/plane.sass";
        planeModel->materialAsset = "/engine_assets/materials/editor_plane.sass";
    }

    const std::shared_ptr<asset::Asset>& Editor::GetSelectedAsset() const
    {
        return m_SelectedAsset;
    }

    void Editor::SelectAsset(const std::shared_ptr<asset::Asset>& asset, bool force)
    {
        m_SelectedEntity = ecs::InvalidEntity;
        m_SelectedSingletonComp = nullptr;
        m_SelectedAsset = asset;
        m_ForceSelection = force;
    }
}
