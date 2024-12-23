#pragma once
#include "startup/StartupManager.h"
#include "ui/OutlineWindow.h"

namespace se::editor
{
    class EditorRuntime
    {
    public:
        void Init();
        void Update();
        void Shutdown();

        void OnEntitiesChanged() const;

    private:
        startup::StartupManager m_StartupManager;

        ui::OutlineWindow m_OutlineWindow;
    };
}
