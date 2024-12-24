#pragma once
#include <ui/PropertiesWindow.h>
#include "startup/StartupManager.h"
#include "ui/OutlineWindow.h"
#include "engine/ecs/World.h"

namespace se::editor
{
    class EditorRuntime
    {
    public:
        void Init();
        void Update();
        void Shutdown();

        ecs::Id GetSelectedEntity() const;
        void SelectEntity(const ecs::Id& id);
        void OnEntitiesChanged() const;

    private:
        startup::StartupManager m_StartupManager;

        ui::OutlineWindow* m_OutlineWindow = nullptr;
        ui::PropertiesWindow* m_PropertiesWindow = nullptr;

        ecs::Id m_SelectedEntity = ecs::s_InvalidEntity;
    };
}
