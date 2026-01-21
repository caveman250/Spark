#pragma once

#include "ui/AssetBrowserWindow.h"
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
        const ecs::Id& GetSelectedEntity() const;
        void SelectEntity(const ecs::Id& id);
        reflect::ObjectBase* GetSelectedSingletonComponent() const;
        void SelectSingletonComponent(reflect::ObjectBase* comp);
        const std::shared_ptr<asset::Asset>& GetSelectedAsset() const;
        void SelectAsset(const std::shared_ptr<asset::Asset>& asset);
        void OnEntitiesChanged() const;

        size_t GetOffscreenRenderGroup() const { return m_OffscreenRenderGroup; }
        const std::shared_ptr<render::FrameBuffer>& GetFrameBuffer() const { return m_FrameBuffer; }
        se::ui::Rect GetViewportRect() const { return m_ViewportWindow->GetViewportScreenspaceRect(); }
        void OnViewportSizeChanged(int x, int y);

        bool InGameMode() const { return m_GameMode; }

    private:
        void CreateGizmo();
        void UpdateSelectedEntityTranslation(const math::Vec3& worldPos) const;
        void SnapGizmoToSelectedEntity() const;
        void HideGizmo() const;
        startup::StartupManager m_StartupManager;

        ui::OutlineWindow* m_OutlineWindow = nullptr;
        ui::PropertiesWindow* m_PropertiesWindow = nullptr;
        ui::ViewportWindow* m_ViewportWindow = nullptr;
        ui::AssetBrowserWindow* m_AssetBrowserWindow = nullptr;

        ecs::Id m_EditorScene = ecs::s_InvalidEntity;
        ecs::Id m_Gizmo = ecs::s_InvalidEntity;

        ecs::Id m_LastSelectedEntity = ecs::s_InvalidEntity;
        ecs::Id m_SelectedEntity = ecs::s_InvalidEntity;
        reflect::ObjectBase* m_SelectedSingletonComp = nullptr;
        reflect::ObjectBase* m_LastSelectedSingletonComp = nullptr;
        std::shared_ptr<asset::Asset> m_SelectedAsset = nullptr;
        std::shared_ptr<asset::Asset> m_LastSelectedAsset = nullptr;

        size_t m_OffscreenRenderGroup = 0;
        std::shared_ptr<render::FrameBuffer> m_FrameBuffer = nullptr;

        bool m_GameMode = false;
    };
}
