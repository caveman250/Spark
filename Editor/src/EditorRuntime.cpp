#include "EditorRuntime.h"

namespace se::editor
{
    void EditorRuntime::Init()
    {
        m_StartupManager.RunStartupTasks();


        m_OutlineWindow.ConstructUI();
    }

    void EditorRuntime::Update()
    {
    }

    void EditorRuntime::Shutdown()
    {
    }

    void EditorRuntime::OnEntitiesChanged() const
    {
        m_OutlineWindow.RebuildOutline();
    }
}
