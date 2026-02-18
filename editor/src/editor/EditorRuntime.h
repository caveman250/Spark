#pragma once

#include "ui/asset_browser/AssetBrowserWindow.h"
#include "ui/ViewportWindow.h"
#include "ui/PropertiesWindow.h"
#include "startup/StartupManager.h"
#include "ui/OutlineWindow.h"
#include "engine/ecs/World.h"

namespace se::asset
{
    class Asset;
}

namespace se::render
{
    class FrameBuffer;
}

namespace se::editor
{
    class EditorRuntime
    {
    public:
        void Init();
        void Update();
        void Render();
        void Shutdown();

        const ecs::Id& GetEditorScene() const;
        const ecs::Id& GetLoadedScene() const;
        void LoadScene(const std::string& path);
        const ecs::Id& GetSelectedEntity() const;
        void SelectEntity(const ecs::Id& id, bool force = false);
        reflect::ObjectBase* GetSelectedSingletonComponent() const;
        void SelectSingletonComponent(reflect::ObjectBase* comp, bool force = false);
        const std::shared_ptr<asset::Asset>& GetSelectedAsset() const;
        void SelectAsset(const std::shared_ptr<asset::Asset>& asset, bool force = false);
        void OnEntitiesChanged() const;

        size_t GetOffscreenRenderGroup() const { return m_OffscreenRenderGroup; }
        const std::shared_ptr<render::FrameBuffer>& GetFrameBuffer() const { return m_FrameBuffer; }
        se::ui::Rect GetViewportRect() const { return m_ViewportWindow->GetViewportScreenspaceRect(); }
        void OnViewportSizeChanged(int x, int y);

        void ToggleGameMode();
        bool InGameMode() const { return m_GameMode; }

        static std::string DuplicateAsset(const std::shared_ptr<asset::Asset>& asset);
        static void SaveAsset(const std::shared_ptr<asset::Asset>& asset);
        static void DeleteAsset(const std::shared_ptr<asset::Asset>& asset);
        void RenameAsset(const std::shared_ptr<asset::Asset>& asset, const std::string& newPath);

    private:
        void SaveAll();
        void SaveScene();
        void CreateGizmo();
        void UpdateSelectedEntityTranslation(const math::Vec3& worldPos) const;
        void SnapGizmoToSelectedEntity() const;
        void HideGizmo() const;
        void CreateEditorPlane();

        startup::StartupManager m_StartupManager;

        ui::OutlineWindow* m_OutlineWindow = nullptr;
        ui::PropertiesWindow* m_PropertiesWindow = nullptr;
        ui::ViewportWindow* m_ViewportWindow = nullptr;
        ui::asset_browser::AssetBrowserWindow* m_AssetBrowserWindow = nullptr;

        ecs::Id m_LoadedScene = ecs::s_InvalidEntity;
        std::string m_ScenePath = {};
        ecs::Id m_EditorScene = ecs::s_InvalidEntity;
        ecs::Id m_Gizmo = ecs::s_InvalidEntity;
        ecs::Id m_Plane = ecs::s_InvalidEntity;

        ecs::Id m_LastSelectedEntity = ecs::s_InvalidEntity;
        ecs::Id m_SelectedEntity = ecs::s_InvalidEntity;
        reflect::ObjectBase* m_SelectedSingletonComp = nullptr;
        reflect::ObjectBase* m_LastSelectedSingletonComp = nullptr;
        std::shared_ptr<asset::Asset> m_SelectedAsset = nullptr;
        std::shared_ptr<asset::Asset> m_LastSelectedAsset = nullptr;
        bool m_ForceSelection = false;

        size_t m_OffscreenRenderGroup = 0;
        std::shared_ptr<render::FrameBuffer> m_FrameBuffer = nullptr;

        bool m_GameMode = false;
    };
}
