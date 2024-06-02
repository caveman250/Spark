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
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsed_seconds = now-m_TimeLastFrame;
        m_DeltaTime = elapsed_seconds.count();
        m_TimeLastFrame = now;

        Application* app = Application::Get();
        app->Update(m_DeltaTime);
        app->Render();
    }
}