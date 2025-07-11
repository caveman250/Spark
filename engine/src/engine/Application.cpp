#include "Application.h"
#include "engine/ui/systems/UIRenderSystem.h"
#include "engine/ui/systems/ScrollBoxRenderSystem.h"
#include "engine/ui/observers/ScrollBoxObserver.h"
#include "engine/ui/systems/WidgetVisibilitySystem.h"
#include "engine/ui/systems/ResetMouseInputSystem.h"
#include "engine/ui/singleton_components/UIRenderComponent.h"
#include "ecs/systems/RootTransformSystem.h"
#include "ecs/systems/TransformSystem.h"
#include "ecs/systems/WorldTransformSystem.h"
#include "input/InputComponent.h"
#include "input/InputSystem.h"
#include "platform/IWindow.h"
#include "render/Renderer.h"
#include "io/VFS.h"
#include "profiling/Profiler.h"
#include "render/systems/MeshRenderSystem.h"
#include "render/systems/PointLightSystem.h"
#include "ui/components/TreeNodeComponent.h"
#include "ui/observers/ButtonObserver.h"
#include "ui/observers/RectTransformObserver.h"
#include "ui/observers/TitleBarObserver.h"
#include "ui/systems/ImageRenderSystem.h"
#include "ui/systems/RootRectTransformSystem.h"
#include "ui/systems/TextRenderSystem.h"
#include "ui/systems/ButtonSystem.h"
#include "ui/systems/TitleBarSystem.h"
#include "ui/systems/TreeViewSystem.h"
#include "ui/systems/UIKeyboardInputSystem.h"
#include "ui/systems/UIMouseInputSystem.h"
#include "engine/ui/systems/TreeNodeSystem.h"
#include "engine/ui/observers/TreeNodeObserver.h"
#include "engine/ui/observers/ImageObserver.h"
#include "engine/ui/observers/TextObserver.h"
#include "ComponentRegistration.generated.h"
#include "debug/components/FPSCounterComponent.h"
#include "debug/systems/FPSCounterSystem.h"
#include "ui/components/TextCaretComponent.h"
#include "ui/observers/EditableTextObserver.h"
#include "ui/systems/EditableTextSystem.h"
#include "ui/systems/ResetKeyInputSystem.h"
#include "ui/systems/TextCaretSystem.h"
#include "engine/ui/systems/ScrollViewUpdateSystem.h"
#include "engine/ui/systems/VerticalBoxSystem.h"
#include "engine/ui/systems/HorizontalBoxSystem.h"
#include "engine/ui/systems/GridBoxSystem.h"
#include "engine/ui/systems/RectTransformSystem.h"
#include "engine/ui/systems/LastRectSystem.h"

#include "Widgets.generated.h"
#include "generated/Classes.generated.h"
#include "camera/ActiveCameraComponent.h"
#include "ecs/components/MeshComponent.h"
#include "render/components/PointLightComponent.h"
#include "ui/systems/CollapsingHeaderSystem.h"
#include "ui/systems/ComboBoxSystem.h"
#include "ui/systems/WindowSystem.h"

namespace se
{
    Application* Application::s_Instance = nullptr;

    // ReSharper disable once CppDFAConstantFunctionResult
    Application* Application::Get()
    {
        return s_Instance;
    }

    void Application::Init()
    {
        PROFILE_BEGIN_FRAME()
        PROFILE_BEGIN_THREAD()

        render::Renderer::Create();
        m_PrimaryWindow = IWindow::CreatePlatformWindow(1280, 720);
        m_TimeLastFrame = std::chrono::system_clock::now();

        io::VFS::Get().Mount(std::format("{}/{}", ENGINE_DIR, "built"), "/engine_assets");
        io::VFS::Get().Mount(std::format("{}/{}", APP_DIR, "built"), "/assets");

        RegisterComponents(&m_World);
        engine_InitClassReflection();

#if SPARK_EDITOR
        m_EditorRuntime.Init();
#endif

        CreateInitialSingletonComponents();
        CreateInitialObservers();
        CreateInitialSystems();
        m_World.Init();
    }

    void Application::CreateInitialSingletonComponents()
    {
        m_World.AddSingletonComponent<input::InputComponent>();
        m_World.AddSingletonComponent<camera::ActiveCameraComponent>();
        m_World.AddSingletonComponent<ui::singleton_components::UIRenderComponent>();
    }

    void Application::CreateInitialObservers()
    {
        m_World.CreateObserver<ui::observers::ButtonObserver, ui::components::ButtonComponent>();
        m_World.CreateObserver<ui::observers::RectTransformObserver, ui::components::RectTransformComponent>();
        m_World.CreateObserver<ui::observers::TitleBarObserver, ui::components::TitleBarComponent>();
        m_World.CreateObserver<ui::observers::TreeNodeObserver, ui::components::TreeNodeComponent>();
        m_World.CreateObserver<ui::observers::ImageObserver, ui::components::ImageComponent>();
        m_World.CreateObserver<ui::observers::TextObserver, ui::components::TextComponent>();
        m_World.CreateObserver<ui::observers::EditableTextObserver, ui::components::EditableTextComponent>();
        m_World.CreateObserver<ui::observers::ScrollBoxObserver, ui::components::ScrollBoxComponent>();
    }

    void Application::CreateInitialSystems()
    {
        ecs::SystemDeclaration inputReg = ecs::SystemDeclaration("Input System")
                .WithSingletonComponent<input::InputComponent>();
        auto input = m_World.CreateEngineSystem<input::InputSystem>(inputReg);

        ecs::SystemDeclaration resetMouseInputReg = ecs::SystemDeclaration("Reset Mouse Input System")
                .WithComponent<const ui::components::RectTransformComponent>()
                .WithComponent<ui::components::MouseInputComponent>()
                .WithComponent<const ui::components::WidgetComponent>()
                .WithSingletonComponent<input::InputComponent>()
                .WithDependency(input);
        auto resetMouseInput = m_World.CreateEngineSystem<ui::systems::ResetMouseInputSystem>(resetMouseInputReg);

        ecs::SystemDeclaration resetKeyInputReg = ecs::SystemDeclaration("Reset Key Input System")
                .WithComponent<ui::components::KeyInputComponent>()
                .WithDependency(input);
        auto resetKeyInput = m_World.CreateEngineSystem<ui::systems::ResetKeyInputSystem>(resetKeyInputReg);

        ecs::SystemDeclaration mouseInputReg = ecs::SystemDeclaration("UIMouseInputSystem")
                .WithComponent<const RootComponent>()
                .WithComponent<ui::components::MouseInputComponent>()
                .WithSingletonComponent<input::InputComponent>()
                .WithDependency(resetMouseInput);
        auto mouseInput = m_World.CreateEngineSystem<ui::systems::UIMouseInputSystem>(mouseInputReg);

        ecs::SystemDeclaration keyboardInputReg = ecs::SystemDeclaration("UIKeyboardInputSystem")
                .WithComponent<const ui::components::RectTransformComponent>()
                .WithComponent<const RootComponent>()
                .WithComponent<ui::components::KeyInputComponent>()
                .WithSingletonComponent<input::InputComponent>()
                .WithDependency(resetKeyInput);
        m_World.CreateEngineSystem<ui::systems::UIKeyboardInputSystem>(keyboardInputReg);

        ecs::SystemDeclaration pointLightReg = ecs::SystemDeclaration("PointLightSystem")
                .WithComponent<const render::components::PointLightComponent>()
                .WithComponent<const TransformComponent>();
        m_World.CreateEngineSystem<render::systems::PointLightSystem>(pointLightReg);

        ecs::SystemDeclaration meshRenderReg = ecs::SystemDeclaration("MeshRenderSystem")
                .WithComponent<const TransformComponent>()
                .WithComponent<const MeshComponent>()
                .WithSingletonComponent<const camera::ActiveCameraComponent>();
        m_World.CreateEngineSystem<render::systems::MeshRenderSystem>(meshRenderReg);

        ecs::SystemDeclaration rootTransformReg = ecs::SystemDeclaration("RootTransformSystem")
                .WithComponent<TransformComponent>()
                .WithComponent<const RootComponent>();
        auto rootTransform = m_World.CreateEngineSystem<ecs::systems::RootTransformSystem>(rootTransformReg);

        ecs::SystemDeclaration worldTransformReg = ecs::SystemDeclaration("WorldTransformSystem")
                .WithComponent<TransformComponent>()
                .WithComponent<ParentComponent>()
                .WithDependency(rootTransform);
        auto worldTransform = m_World.CreateEngineSystem<ecs::systems::WorldTransformSystem>(worldTransformReg);

        ecs::SystemDeclaration transformReg = ecs::SystemDeclaration("TransformSystem")
                .WithComponent<TransformComponent>()
                .WithDependency(worldTransform);
        m_World.CreateEngineSystem<ecs::systems::TransformSystem>(transformReg);

        ecs::SystemDeclaration lastRectReg = ecs::SystemDeclaration("LastRectSystem")
                .WithComponent<ui::components::RectTransformComponent>();
        auto lastRect = m_World.CreateEngineSystem<ui::systems::LastRectSystem>(lastRectReg);

        ecs::SystemDeclaration rootRectReg = ecs::SystemDeclaration("RootRectTransformSystem")
                .WithComponent<ui::components::RectTransformComponent>()
                .WithComponent<const RootComponent>()
                .WithDependency(lastRect);
        auto rootRect = m_World.CreateEngineSystem<ui::systems::RootRectTransformSystem>(rootRectReg);

        ecs::SystemDeclaration rectTransformReg = ecs::SystemDeclaration("RectTransformSystem")
                .WithComponent<ui::components::RectTransformComponent>()
                .WithDependency(rootRect);
        auto rectTrans = m_World.CreateEngineSystem<ui::systems::RectTransformSystem>(rectTransformReg);

        ecs::SystemDeclaration treeNodesReg = ecs::SystemDeclaration("TreeNodeSystem")
                .WithComponent<ui::components::TreeNodeComponent>()
                .WithComponent<ui::components::MouseInputComponent>()
                .WithDependency(mouseInput);
        auto treeNodes = m_World.CreateEngineSystem<ui::systems::TreeNodeSystem>(treeNodesReg);

        ecs::SystemDeclaration treeViewReg = ecs::SystemDeclaration("TreeViewSystem")
                .WithComponent<ui::components::TreeViewComponent>()
                .WithComponent<ui::components::RectTransformComponent>()
                .WithDependency(rectTrans)
                .WithDependency(treeNodes)
                .WithHeirachyQuery(ui::components::TreeNodeComponent::GetComponentId(), ecs::ComponentMutability::Immutable)
                .WithHeirachyQuery(ui::components::RectTransformComponent::GetComponentId(), ecs::ComponentMutability::Mutable)
                .WithHeirachyQuery(ui::components::WidgetComponent::GetComponentId(), ecs::ComponentMutability::Mutable);
        m_World.CreateEngineSystem<ui::systems::TreeViewSystem>(treeViewReg);

        ecs::SystemDeclaration buttonReg = ecs::SystemDeclaration("ButtonSystem")
                .WithComponent<ui::components::ButtonComponent>()
                .WithComponent<ui::components::ImageComponent>()
                .WithComponent<const ui::components::MouseInputComponent>()
                .WithDependency(mouseInput);
        m_World.CreateEngineSystem<ui::systems::ButtonSystem>(buttonReg);

        ecs::SystemDeclaration titleBarReg = ecs::SystemDeclaration("TitleBarSystem")
                .WithComponent<ui::components::TitleBarComponent>()
                .WithComponent<const ui::components::MouseInputComponent>()
                .WithSingletonComponent<input::InputComponent>()
                .WithDependency(mouseInput);
        m_World.CreateEngineSystem<ui::systems::TitleBarSystem>(titleBarReg);

        ecs::SystemDeclaration windowReg = ecs::SystemDeclaration("Window System")
                .WithComponent<ui::components::WindowComponent>()
                .WithComponent<ui::components::RectTransformComponent>()
                .WithComponent<const ui::components::MouseInputComponent>()
                .WithSingletonComponent<const input::InputComponent>()
                .WithDependency(mouseInput);
        m_World.CreateEngineSystem<ui::systems::WindowSystem>(windowReg);

        ecs::SystemDeclaration scrollViewReg = ecs::SystemDeclaration("ScrollViewUpdateSystem")
                .WithComponent<ui::components::ScrollViewComponent>()
                .WithComponent<ui::components::RectTransformComponent>()
                .WithComponent<const ui::components::MouseInputComponent>()
                .WithDependency(mouseInput)
                .WithDependency(rectTrans)
                .WithHeirachyQuery(ui::components::WidgetComponent::GetComponentId(), ecs::ComponentMutability::Mutable);
        auto scrollView = m_World.CreateEngineSystem<ui::systems::ScrollViewUpdateSystem>(scrollViewReg);

        ecs::SystemDeclaration verticalBoxReg = ecs::SystemDeclaration("Vertical Box System")
                .WithComponent<ui::components::VerticalBoxComponent>()
                .WithComponent<ui::components::RectTransformComponent>()
                .WithComponent<const ui::components::WidgetComponent>()
                .WithDependency(rectTrans)
                .WithDependency(scrollView)
                .WithHeirachyQuerys<SPARK_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable);
        m_World.CreateEngineSystem<ui::systems::VerticalBoxSystem>(verticalBoxReg);

        ecs::SystemDeclaration horizontalBoxReg = ecs::SystemDeclaration("Horizontal Box System")
                .WithComponent<ui::components::HorizontalBoxComponent>()
                .WithComponent<ui::components::RectTransformComponent>()
                .WithComponent<const ui::components::WidgetComponent>()
                .WithDependency(rectTrans)
                .WithDependency(scrollView)
                .WithHeirachyQuerys<SPARK_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable);
        m_World.CreateEngineSystem<ui::systems::HorizontalBoxSystem>(horizontalBoxReg);

        ecs::SystemDeclaration gridBoxReg = ecs::SystemDeclaration("Grid Box System")
                .WithComponent<ui::components::GridBoxComponent>()
                .WithComponent<ui::components::RectTransformComponent>()
                .WithHeirachyQuery<const ui::components::WidgetComponent>()
                .WithDependency(rectTrans)
                .WithDependency(scrollView)
                .WithHeirachyQuerys<SPARK_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable);
        m_World.CreateEngineSystem<ui::systems::GridBoxSystem>(gridBoxReg);

        ecs::SystemDeclaration imageRenderReg = ecs::SystemDeclaration("ImageRenderSystem")
                .WithComponent<const ui::components::RectTransformComponent>()
                .WithComponent<ui::components::ImageComponent>()
                .WithComponent<const ui::components::WidgetComponent>()
                .WithSingletonComponent<ui::singleton_components::UIRenderComponent>();
        auto imageRender = m_World.CreateEngineSystem<ui::systems::ImageRenderSystem>(imageRenderReg);

        ecs::SystemDeclaration textRenderReg = ecs::SystemDeclaration("TextRenderSystem")
                .WithComponent<const ui::components::RectTransformComponent>()
                .WithComponent<ui::components::TextComponent>()
                .WithComponent<const ui::components::WidgetComponent>()
                .WithSingletonComponent<ui::singleton_components::UIRenderComponent>();
        auto textRender = m_World.CreateEngineSystem<ui::systems::TextRenderSystem>(textRenderReg);

        ecs::SystemDeclaration editableTextRenderReg = ecs::SystemDeclaration("EditableTextRenderSystem")
                .WithComponent<const ui::components::RectTransformComponent>()
                .WithComponent<ui::components::EditableTextComponent>()
                .WithComponent<ui::components::WidgetComponent>()
                .WithComponent<const ui::components::MouseInputComponent>()
                .WithComponent<ui::components::KeyInputComponent>()
                .WithSingletonComponent<ui::singleton_components::UIRenderComponent>()
                .WithSingletonComponent<const input::InputComponent>()
                .WithHeirachyQuery<ui::components::TextCaretComponent>()
                .WithDependency(input);
        auto editableText = m_World.CreateEngineSystem<ui::systems::EditableTextSystem>(editableTextRenderReg);

        ecs::SystemDeclaration scrollBoxRenderReg = ecs::SystemDeclaration("ScrollBoxRenderSystem")
                .WithComponent<ui::components::ScrollBoxComponent>()
                .WithComponent<const ui::components::RectTransformComponent>()
                .WithSingletonComponent<ui::singleton_components::UIRenderComponent>();
        auto scrollBoxRender = m_World.CreateEngineSystem<ui::systems::ScrollBoxRenderSystem>(scrollBoxRenderReg);

        ecs::SystemDeclaration widgetVisibilityReg = ecs::SystemDeclaration("WidgetVisibilitySystem")
                .WithComponent<ui::components::WidgetComponent>();
        m_World.CreateEngineSystem<ui::systems::WidgetVisibilitySystem>(widgetVisibilityReg);

#if !SPARK_DIST
        ecs::SystemDeclaration fpsCounterReg = ecs::SystemDeclaration("FPS Counter System")
                .WithComponent<ui::components::TextComponent>()
                .WithComponent<debug::components::FPSCounterComponent>()
                .WithDependency(rectTrans);
        m_World.CreateEngineSystem<debug::systems::FPSCounterSystem>(fpsCounterReg);
#endif

        ecs::SystemDeclaration textCaretReg = ecs::SystemDeclaration("Text Caret System")
                .WithComponent<ui::components::TextCaretComponent>()
                .WithComponent<ui::components::WidgetComponent>();
        m_World.CreateEngineSystem<ui::systems::TextCaretSystem>(textCaretReg);

        ecs::SystemDeclaration collapsingHeaderReg = ecs::SystemDeclaration("Collapsing Header System")
                .WithComponent<ui::components::CollapsingHeaderComponent>()
                .WithComponent<ui::components::RectTransformComponent>()
                .WithComponent<const ui::components::MouseInputComponent>()
                .WithHeirachyQuery<ui::components::WidgetComponent>()
                .WithDependency(rootRect);
        auto collapsingHeader = m_World.CreateEngineSystem<ui::systems::CollapsingHeaderSystem>(collapsingHeaderReg);

        ecs::SystemDeclaration comboBoxReg = ecs::SystemDeclaration("ComboBox System")
                .WithComponent<ui::components::ComboBoxComponent>()
                .WithComponent<ui::components::RectTransformComponent>()
                .WithComponent<const ui::components::MouseInputComponent>()
                .WithHeirachyQuery<ui::components::WidgetComponent>()
                .WithHeirachyQuerys<SPARK_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable)
                .WithDependency(mouseInput)
                .WithDependency(rootRect);
        m_World.CreateEngineSystem<ui::systems::ComboBoxSystem>(comboBoxReg);

        // TODO allow application to add dependencies here.
        ecs::SystemDeclaration uiRenderReg = ecs::SystemDeclaration("UIRenderSystem")
                .WithComponent<const RootComponent>()
                .WithComponent<const ui::components::WidgetComponent>()
                .WithSingletonComponent<ui::singleton_components::UIRenderComponent>()
                .WithDependency(imageRender)
                .WithDependency(textRender)
                .WithDependency(editableText)
                .WithDependency(scrollBoxRender)
                .WithDependency(collapsingHeader);
        m_World.CreateEngineSystem<ui::systems::UIRenderSystem>(uiRenderReg);
    }

    void Application::Shutdown()
    {
    }

    math::IntVec2 Application::GetGameViewportSize()
    {
#if SPARK_EDITOR
        return m_EditorRuntime.GetFrameBuffer()->GetSize() * m_PrimaryWindow->GetContentScale();
#else
        return math::IntVec2(m_PrimaryWindow->GetWidth(), m_PrimaryWindow->GetHeight());
#endif
    }

    void Application::Update()
    {
        PROFILE_SCOPE("Application::Update")
        const auto now = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsed_seconds = now - m_TimeLastFrame;
        m_DeltaTime = elapsed_seconds.count();
        m_TimeLastFrame = now;

        m_World.Update();

#if SPARK_EDITOR
        m_EditorRuntime.Update();
#endif
    }

    void Application::Render()
    {
        PROFILE_SCOPE("Application::Render")

#if SPARK_EDITOR
        m_EditorRuntime.Render();
#endif

        m_World.Render();
    }
}
