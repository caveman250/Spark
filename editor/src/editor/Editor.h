#pragma once

#include "EditorMode.h"
#include "GizmoManager.h"
#include "engine/ecs/Prefab.h"
#include "singleton_components/EditorShortcutsComponent.h"
#include "ui/asset_browser/AssetBrowserWindow.h"
#include "ui/ViewportWindow.h"
#include "ui/PropertiesWindow.h"
#include "startup/StartupManager.h"
#include "ui/OutlineWindow.h"
#include "engine/ecs/World.h"

namespace se::asset
{
    struct StaticMesh;
    class Asset;
}

namespace se::render
{
    class FrameBuffer;
}

namespace se::editor
{
    namespace components
    {
        enum class RotationAxis;
    }

    class Editor
    {
    public:
        void Init();
        void Update();
        void Render();
        void Shutdown();

        EditorMode GetMode() const { return m_Mode; }
        const ecs::Id& GetEditorScene() const;
        const ecs::Id& GetPrefabEditorScene() const;
        const ecs::Id& GetLoadedScene() const;
        void LoadScene(const std::string& path);
        void EditPrefab(const std::shared_ptr<asset::Asset>& asset);
        const ecs::Id& GetSelectedEntity() const;
        void SelectEntity(const ecs::Id& id, bool force = false);
        reflect::ObjectBase* GetSelectedSingletonComponent() const;
        void SelectSingletonComponent(reflect::ObjectBase* comp, bool force = false);
        const std::shared_ptr<asset::Asset>& GetSelectedAsset() const;
        void SelectAsset(const std::shared_ptr<asset::Asset>& asset, bool force = false);
        void DeSelectAll();
        void OnEntitiesChanged() const;

        void Copy();
        void Cut();
        void Paste();
        bool HasValidCopySelection() const;
        bool HasValidPasteTarget() const;

        size_t GetOffscreenRenderGroup() const { return m_OffscreenRenderGroup; }
        const std::shared_ptr<render::FrameBuffer>& GetFrameBuffer() const { return m_FrameBuffer; }
        size_t GetPrefabRenderGroup() const { return m_PrefabRenderGroup; }
        const std::shared_ptr<render::FrameBuffer>& GetPrefabFrameBuffer() const { return m_PrefabFrameBuffer; }
        se::ui::Rect GetViewportRect() const { return m_ViewportWindow->GetViewportScreenspaceRect(); }
        void OnViewportSizeChanged(int x, int y);

        void ToggleGameMode();
        bool InGameMode() const { return m_GameMode; }

        GizmoManager& GetGizmoManager() { return m_GizmoManager; }

        static std::string DuplicateAsset(const std::shared_ptr<asset::Asset>& asset);
        static void SaveAsset(const std::shared_ptr<asset::Asset>& asset);
        void DeleteAsset(const std::shared_ptr<asset::Asset>& asset);
        void RestoreAsset(const std::shared_ptr<asset::Asset>& asset, const std::shared_ptr<asset::meta::MetaData>& meta);
        void RenameAsset(const std::shared_ptr<asset::Asset>& asset, const std::string& newPath);
        void SaveAll();

    private:
        void SaveScene();
        void CreateEditorPlane();
        void ExitPrefabMode();

        EditorMode m_Mode = {};
        startup::StartupManager m_StartupManager = {};
        GizmoManager m_GizmoManager = {};

        ui::OutlineWindow* m_OutlineWindow = nullptr;
        ui::PropertiesWindow* m_PropertiesWindow = nullptr;
        ui::ViewportWindow* m_ViewportWindow = nullptr;
        std::shared_ptr<ui::asset_browser::AssetBrowserWindow> m_AssetBrowserWindow = nullptr;

        ecs::Id m_LoadedScene = ecs::InvalidEntity;
        std::string m_ScenePath = {};
        ecs::Id m_EditorScene = ecs::InvalidEntity;
        ecs::Id m_PrefabEditorScene = ecs::InvalidEntity;
        ecs::Id m_Plane = ecs::InvalidEntity;

        ecs::Id m_LastSelectedEntity = ecs::InvalidEntity;
        ecs::Id m_SelectedEntity = ecs::InvalidEntity;
        reflect::ObjectBase* m_SelectedSingletonComp = nullptr;
        reflect::ObjectBase* m_LastSelectedSingletonComp = nullptr;
        std::shared_ptr<asset::Asset> m_SelectedAsset = nullptr;
        std::shared_ptr<asset::Asset> m_LastSelectedAsset = nullptr;
        bool m_ForceSelection = false;

        size_t m_OffscreenRenderGroup = 0;
        std::shared_ptr<render::FrameBuffer> m_FrameBuffer = nullptr;
        size_t m_PrefabRenderGroup = 0;
        std::shared_ptr<render::FrameBuffer> m_PrefabFrameBuffer = nullptr;

        bool m_GameMode = false;

        // cut/copy/paste
        ecs::Id m_EntityToCopy = ecs::InvalidEntity;
        ecs::Prefab m_CutEntity = {};
        ecs::Id m_EditingPrefabRoot = {};
        std::shared_ptr<asset::Asset> m_EditingPrefabAsset = {};
    };
}
