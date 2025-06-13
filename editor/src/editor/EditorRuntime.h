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

        ecs::Id GetSelectedEntity() const;
        void SelectEntity(const ecs::Id& id);
        reflect::ObjectBase* GetSelectedSingletonComponent() const;
        void SelectSingletonComponent(reflect::ObjectBase* comp);
        const std::shared_ptr<asset::Asset>& GetSelectedAsset() const;
        void SelectAsset(const std::shared_ptr<asset::Asset>& asset);
        void OnEntitiesChanged() const;

        size_t GetOffscreenRenderGroup() const { return m_OffscreenRenderGroup; }
        const std::shared_ptr<render::FrameBuffer>& GetFrameBuffer() const { return m_FrameBuffer; }
        se::ui::Rect GetViewportRect() const { return m_ViewportWindow->GetViewportScreenspaceRect(); }

    private:
        startup::StartupManager m_StartupManager;

        ui::OutlineWindow* m_OutlineWindow = nullptr;
        ui::PropertiesWindow* m_PropertiesWindow = nullptr;
        ui::ViewportWindow* m_ViewportWindow = nullptr;
        ui::AssetBrowserWindow* m_AssetBrowserWindow = nullptr;

        ecs::Id m_SelectedEntity = ecs::s_InvalidEntity;
        reflect::ObjectBase* m_SelectedSingletonComp = nullptr;
        std::shared_ptr<asset::Asset> m_SelectedAsset = nullptr;

        size_t m_OffscreenRenderGroup = 0;
        std::shared_ptr<render::FrameBuffer> m_FrameBuffer = nullptr;
    };
}
