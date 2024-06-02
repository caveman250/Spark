#include "PlatformRunLoop.h"
#include "engine/Application.h"

namespace se
{
    PlatformRunLoop* PlatformRunLoop::Get()
    {
        return s_Instance;
    }

    void PlatformRunLoop::Update()
    {
        Application* app = Application::Get();
        app->Update();
        app->Render();
    }
}