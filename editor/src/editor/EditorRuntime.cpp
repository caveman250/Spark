#include "EditorRuntime.h"
#include "engine/render/Renderer.h"
#include "../generated/Classes.generated.h"

namespace se::editor
{
    void EditorRuntime::Init()
    {
        editor_InitClassReflection();

        m_StartupManager.RunStartupTasks();

        m_FrameBuffer = render::FrameBuffer::CreateFrameBuffer();

        m_OutlineWindow = new ui::OutlineWindow(this);
        m_OutlineWindow->ConstructUI();

        m_PropertiesWindow = new ui::PropertiesWindow(this);
        m_PropertiesWindow->ConstructUI();

        m_ViewportWindow = new ui::ViewportWindow(this);
        m_ViewportWindow->ConstructUI();

        m_AssetBrowserWindow = new ui::AssetBrowserWindow(this);
        m_AssetBrowserWindow->ConstructUI();
    }

    void EditorRuntime::Update()
    {
        m_PropertiesWindow->Update();
        m_ViewportWindow->Update();
    }

    void EditorRuntime::Render()
    {
        auto renderer = render::Renderer::Get<render::Renderer>();
        m_OffscreenRenderGroup = renderer->AllocRenderGroup(0);
        renderer->SetFrameBuffer(m_OffscreenRenderGroup, m_FrameBuffer);

        renderer->Submit<render::commands::Clear>(m_OffscreenRenderGroup, true, true);
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
        m_SelectedAsset = nullptr;
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
        m_SelectedAsset = nullptr;
        m_PropertiesWindow->RebuildProperties();
    }

    void EditorRuntime::OnEntitiesChanged() const
    {
        m_OutlineWindow->RebuildOutline();
    }

    const std::shared_ptr<asset::Asset>& EditorRuntime::GetSelectedAsset() const
    {
        return m_SelectedAsset;
    }

    void EditorRuntime::SelectAsset(const std::shared_ptr<asset::Asset>& asset)
    {
        m_SelectedEntity = ecs::s_InvalidEntity;
        m_SelectedSingletonComp = nullptr;
        m_SelectedAsset = asset;
        m_PropertiesWindow->RebuildProperties();
    }
}
