#include "PlatformRunLoop.h"
#include "engine/Application.h"
#include "engine/input/InputComponent.h"
#include "engine/profiling/Profiler.h"
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
        auto app = Application::Get();
        app->Init();
        m_Window = Application::Get()->GetPrimaryWindow();
    }

    void PlatformRunLoop::Shutdown()
    {
        auto app = Application::Get();
        app->Shutdown();
    }

    void PlatformRunLoop::Update()
    {
        PROFILE_SCOPE("PlatformRunLoop::Update")
        Application* app = Application::Get();

        render::Renderer::Get<render::Renderer>()->Update();
        app->Update();
        app->Render();

        auto inputComp = app->GetWorld()->GetSingletonComponent<input::InputComponent>();
        inputComp->keyEvents.clear();
        inputComp->mouseEvents.clear();
    }
}
