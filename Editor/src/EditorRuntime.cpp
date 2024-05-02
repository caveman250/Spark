#include "EditorRuntime.h"

namespace se::editor
{
    void EditorRuntime::Init()
    {
        m_StartupManager.RunStartupTasks();
    }

    void EditorRuntime::Update()
    {
    }

    void EditorRuntime::Shutdown()
    {
    }
}
