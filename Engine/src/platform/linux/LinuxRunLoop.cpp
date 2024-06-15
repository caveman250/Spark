#include "LinuxRunLoop.h"

#include "KeyMap.h"
#include "Window.h"
#include "engine/Application.h"
#include "engine/input/InputComponent.h"
#include "engine/input/Key.h"
#include "engine/render/Renderer.h"

namespace se
{
    PlatformRunLoop* PlatformRunLoop::s_Instance = nullptr;

    PlatformRunLoop* PlatformRunLoop::CreatePlatformRunloop(std::vector<IWindow*> windows)
    {
        return new linux::LinuxRunLoop(windows);
    }
}

namespace se::linux
{
    LinuxRunLoop::LinuxRunLoop(std::vector<IWindow*> windows)
    {
        std::ranges::for_each(windows, [this](IWindow* window)
        {
            RegisterWindow(window);
        });
    }

    void LinuxRunLoop::Update()
    {
        auto inputComp = Application::Get()->GetWorld()->GetSingletonComponent<input::InputComponent>();

        for (const auto& window : m_Windows)
        {
            SDL_Event ev;
            while (SDL_PollEvent(&ev))
            {
                switch (ev.type)
                {
                case SDL_KEYDOWN:
                    {
                        input::Key::Type key = KeyMap::SDLKeyToSparkKey(ev.key.keysym.scancode);
                        auto keyEvent = input::KeyEvent
                        {
                            .key = key,
                            .state = input::KeyState::Down
                        };
                        inputComp->keyEvents.push_back(keyEvent);
                        inputComp->keyStates[key] = keyEvent.state;
                        break;
                    }
                case SDL_KEYUP:
                    {
                        input::Key::Type key = KeyMap::SDLKeyToSparkKey(ev.key.keysym.scancode);
                        auto keyEvent = input::KeyEvent
                        {
                            .key = key,
                            .state = input::KeyState::Up
                        };
                        inputComp->keyEvents.push_back(keyEvent);
                        inputComp->keyStates[key] = keyEvent.state;
                        break;
                    }
                case SDL_MOUSEBUTTONDOWN:
                    {
                        inputComp->mouseButtonStates[ev.button.button] = input::KeyState::Down;
                        break;
                    }
                case SDL_MOUSEBUTTONUP:
                    {
                        inputComp->mouseButtonStates[ev.button.button] = input::KeyState::Up;
                        break;
                    }
                case SDL_MOUSEMOTION:
                    {
                        inputComp->mouseX = ev.motion.x;
                        inputComp->mouseY = ev.motion.y;
                        break;
                    }
                case SDL_WINDOWEVENT:
                    switch (ev.window.event)
                    {
                        case SDL_WINDOWEVENT_RESIZED:
                            window->OnResize(ev.window.data1, ev.window.data2);
                            break;
                        case SDL_WINDOWEVENT_MOVED:
                            window->OnMove(ev.window.data1, ev.window.data2);
                            break;
                        default:
                            break;//TODO
                    }
                    break;
                case SDL_QUIT:
                    window->OnClose();
                    break;
                }
            }
        }

        PlatformRunLoop::Update();

        for (const auto& window : m_Windows)
        {
            window->SetCurrent();
            render::Renderer::Get()->Render(window);
            SDL_GL_SwapWindow(window->GetSDLWindow());
        }

        render::Renderer::Get()->EndFrame();

        auto safeCopy = m_Windows;
        for (const auto& window : safeCopy)
        {
            if (window->ShouldClose())
            {
                window->Cleanup();
                delete window;
            }
        }
    }

    bool LinuxRunLoop::ShouldExit()
    {
        return m_ShouldExit;
    }

    void LinuxRunLoop::RegisterWindow(IWindow* window)
    {
        Window* platformWindow = dynamic_cast<Window*>(window);
        SPARK_ASSERT(platformWindow);
        m_Windows.push_back(platformWindow);
    }

    void LinuxRunLoop::UnregisterWindow(IWindow* window)
    {
        std::erase(m_Windows, window);
        if (m_Windows.empty())
        {
            m_ShouldExit = true;
        }
    }
}
