#include "EditorRuntime.h"

namespace se::editor
{
    void EditorRuntime::Init()
    {
        m_StartupManager.RunStartupTasks();

        m_OutlineWindow = new ui::OutlineWindow(this);
        m_OutlineWindow->ConstructUI();

        m_PropertiesWindow = new ui::PropertiesWindow(this);
        m_PropertiesWindow->ConstructUI();
    }

    void EditorRuntime::Update()
    {
        m_PropertiesWindow->Update();
    }

    void EditorRuntime::Shutdown()
    {
    }

    ecs::Id EditorRuntime::GetSelectedEntity() const
    {
        return m_SelectedEntity;
    }

    void EditorRuntime::SelectEntity(const ecs::Id& id)
    {
        m_SelectedEntity = id;
        m_PropertiesWindow->RebuildProperties();
    }

    void EditorRuntime::OnEntitiesChanged() const
    {
        m_OutlineWindow->RebuildOutline();
    }
}
