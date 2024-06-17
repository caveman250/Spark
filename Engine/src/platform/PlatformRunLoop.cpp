#include "PlatformRunLoop.h"
#include "engine/Application.h"
#include "engine/input/InputComponent.h"
#include "platform/IWindow.h"

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

    void PlatformRunLoop::Update()
    {
        Application* app = Application::Get();

        app->GetPrimaryWindow()->SetCurrent();
        app->Update();
        app->Render();

        auto inputComp = app->GetWorld()->GetSingletonComponent<input::InputComponent>();
        inputComp->keyEvents.clear();
    }
}
