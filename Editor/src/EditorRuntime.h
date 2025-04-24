#pragma once
#include "ui/PropertiesWindow.h"
#include "startup/StartupManager.h"
#include "ui/OutlineWindow.h"
#include "engine/ecs/World.h"

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
        void OnEntitiesChanged() const;

        size_t GetOffscreenRenderGroup() const { return m_OffscreenRenderGroup; }

    private:
        startup::StartupManager m_StartupManager;

        ui::OutlineWindow* m_OutlineWindow = nullptr;
        ui::PropertiesWindow* m_PropertiesWindow = nullptr;

        ecs::Id m_SelectedEntity = ecs::s_InvalidEntity;
        reflect::ObjectBase* m_SelectedSingletonComp = nullptr;

        size_t m_OffscreenRenderGroup = 0;
        std::shared_ptr<render::FrameBuffer> m_FrameBuffer = nullptr;
    };
}
