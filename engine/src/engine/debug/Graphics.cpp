#include "Graphics.h"

#include "engine/Application.h"
#include "engine/asset/mesh/StaticMesh.h"
#include "engine/render/Renderer.h"
#include "engine/render/metal/VertexBuffer.h"
#include "engine/camera/ActiveCameraComponent.h"
#include "engine/render/MaterialInstance.h"

namespace se::debug
{
    Graphics& Graphics::Get()
    {
        static Graphics* instance = nullptr;
        if (!instance)
        {
            instance = new Graphics();
        }

        return *instance;
    }

    void Graphics::Render()
    {
        if (m_Lines.empty())
        {
            return;
        }

        if (!m_LineMaterial)
        {
            m_LineMaterial = std::make_shared<render::MaterialInstance>("/engine_assets/materials/M_Line.sass");
        }
        auto renderer = render::Renderer::Get<render::Renderer>();
#if SPARK_EDITOR
        auto* editor = Application::Get()->GetEditor();
        const size_t defaultRenderGroup = editor->GetMode() == editor::EditorMode::Prefab ? editor->GetPrefabRenderGroup() : editor->GetOffscreenRenderGroup();
#else
        const size_t defaultRenderGroup = renderer->GetDefaultRenderGroup();
#endif
        std::shared_ptr<render::VertexBuffer> vertBuffer = render::VertexBuffer::CreateVertexBuffer(m_Lines);
        vertBuffer->CreatePlatformResource();
        renderer->Submit<render::commands::SubmitGeo>(defaultRenderGroup, m_LineMaterial, vertBuffer, nullptr, math::Vec3(0.f));

        auto* camera = Application::Get()->GetWorld()->GetSingletonComponent<camera::ActiveCameraComponent>();
        m_LineMaterial->SetUniform("view", 1, &camera->view);
        m_LineMaterial->SetUniform("proj", 1, &camera->proj);

        m_Lines.clear();
    }

    void Graphics::AddLine(const math::Vec3& start,
                           const math::Vec3& end,
                           const math::Vec4& colour)
    {
        auto lock = std::lock_guard(m_LinesMutex);
        m_Lines.push_back({ start, end, colour });
    }
}
