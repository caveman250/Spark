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
        m_SelectedSingletonComp = nullptr;
        m_PropertiesWindow->RebuildProperties();
    }

    reflect::ObjectBase* EditorRuntime::GetSelectedSingletonComponent() const
    {
        return m_SelectedSingletonComp;
    }

    void EditorRuntime::SelectSingletonComponent(reflect::ObjectBase* comp)
    {
        m_SelectedEntity = ecs::s_InvalidEntity;
        m_SelectedSingletonComp = comp;
        m_PropertiesWindow->RebuildProperties();
    }

    void EditorRuntime::OnEntitiesChanged() const
    {
        m_OutlineWindow->RebuildOutline();
    }
}
