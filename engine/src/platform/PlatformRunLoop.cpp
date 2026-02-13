#include "PlatformRunLoop.h"
#include "engine/Application.h"
#include "engine/input/InputComponent.h"
#include <easy/profiler.h>
#include "engine/render/Renderer.h"

namespace se
{
    namespace input
    {
        struct InputComponent;
    }

    PlatformRunLoop* PlatformRunLoop::Get()
    {
        return s_Instance;
    }


    void PlatformRunLoop::Init()
    {
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        auto app = Application::Get();
        app->Init();
        m_Window = Application::Get()->GetWindow();
    }

    void PlatformRunLoop::Shutdown()
    {
        auto app = Application::Get();
        app->Shutdown();
#if SPARK_ENABLE_PROFILING
        profiler::dumpBlocksToFile(std::format("{}/save/profile.prof", APP_DIR).c_str());
#endif
    }

    void PlatformRunLoop::Update()
    {
        EASY_BLOCK("PlatformRunLoop::Update");
        Application* app = Application::Get();

        render::Renderer::Get<render::Renderer>()->Update();
        app->Update();
        app->Render();

        auto inputComp = app->GetWorld()->GetSingletonComponent<input::InputComponent>();
        inputComp->keyEvents.clear();
        inputComp->mouseEvents.clear();

        auto window = app->GetWindow();
        window->SetLastContentScale(window->GetContentScale());
    }
}
