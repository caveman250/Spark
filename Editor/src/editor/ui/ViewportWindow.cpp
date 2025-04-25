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

namespace se::editor::ui
{
    void ViewportWindow::Update()
    {
        ToolWindow::Update();
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
        windowTransform->minX = 430;
        windowTransform->maxX = 1260;
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
        auto assetManager = asset::AssetManager::Get();
        auto vert = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/ui.sass");
        auto frag = assetManager->GetAsset<asset::Shader>("/builtin_assets/shaders/diffuse_texture.sass");
        static auto material = render::Material::CreateMaterial({vert}, {frag});

        imageComp->materialInstance = render::MaterialInstance::CreateMaterialInstance(material);
        const auto& viewportTexture = app->GetEditorRuntime()->GetFrameBuffer()->GetColorTexture();
        imageComp->materialInstance->SetUniform("Texture", asset::shader::ast::AstType::Sampler2D, 1, &viewportTexture);
        world->AddChild(contentArea, m_Viewport);
    }

    void ViewportWindow::DestroyUI()
    {
        auto app = Application::Get();
        auto world = app->GetWorld();

        world->DestroyEntity(m_Window);
        m_Window = ecs::s_InvalidEntity;
    }
}