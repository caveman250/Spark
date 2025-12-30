#include "ViewportWindow.h"
#include "engine/ui/components/WidgetComponent.h"
#include "engine/ui/components/ImageComponent.h"
#include "engine/asset/AssetManager.h"
#include "engine/Application.h"
#include "engine/ui/components/WindowComponent.h"
#include "engine/ui/components/TitleBarComponent.h"
#include "engine/ui/components/RectTransformComponent.h"
#include "engine/ui/util/WindowUtil.h"
#include "engine/render/MaterialInstance.h"
#include "engine/render/FrameBuffer.h"
#include "engine/render/render_fwd.h"

namespace se::editor::ui
{
    void ViewportWindow::Update()
    {
        ToolWindow::Update();
        if (!m_Valid)
        {
            return;
        }

        auto world = Application::Get()->GetWorld();
        se::ui::components::RectTransformComponent* viewportRect = world->GetComponent<se::ui::components::RectTransformComponent>(m_Viewport);
        m_ViewportRect = viewportRect->rect;
    }

    void ViewportWindow::ConstructUI()
    {
        auto app = Application::Get();
        auto world = app->GetWorld();

        se::ui::components::RectTransformComponent *windowTransform;
        se::ui::components::WindowComponent *windowComp;
        se::ui::components::TitleBarComponent *titleBarComp;
        ecs::Id contentArea;
        m_Window = ::se::ui::util::CreateWindow(&windowTransform,
                                                &windowComp,
                                                &titleBarComp,
                                                contentArea,
                                                "Viewport",
                                                [this]()
                                                { DestroyUI(); },
                                                true);
        windowTransform->anchors = {0.f, 0.f, 0.f, 0.f};
        windowTransform->minX = 310;
        windowTransform->maxX = 850;
        windowTransform->minY = 20;
        windowTransform->maxY = 380;

        m_Viewport = world->CreateEntity("Viewport Image", true);
        auto imageTransform = world->AddComponent<se::ui::components::RectTransformComponent>(m_Viewport);
        imageTransform->anchors = { .left = 0.f, .right = 1.f, .top = 0.f, .bottom = 1.f };
        imageTransform->minY = 5;
        imageTransform->maxY = 5;
        imageTransform->minX = 5;
        imageTransform->maxX = 5;
        imageTransform->maxAspectRatio = 1.778f; // 16:9
        imageTransform->minAspectRatio = 1.778f;
        world->AddComponent<se::ui::components::WidgetComponent>(m_Viewport);
        auto imageComp = world->AddComponent<se::ui::components::ImageComponent>(m_Viewport);
        static std::shared_ptr<render::Material> material = nullptr;
        if (!material)
        {
            material = std::make_shared<render::Material>(
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/ui.sass") },
                std::vector{ asset::AssetReference<asset::Shader>("/engine_assets/shaders/framebuffer_texture.sass") });
#if OPENGL_RENDERER
            material->GetShaderSettings().SetSetting("ymod", -1.f);
#else
            material->GetShaderSettings().SetSetting("ymod", 1.f);
#endif
        }



        imageComp->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        const auto& viewportTexture = app->GetEditorRuntime()->GetFrameBuffer()->GetColorTexture();
        imageComp->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &viewportTexture);
        world->AddChild(contentArea, m_Viewport);
        m_Valid = true;
    }

    void ViewportWindow::DestroyUI()
    {
        m_Valid = false;
    }
}