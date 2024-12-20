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
        s_Instance = new linux::LinuxRunLoop(windows);
        return s_Instance;
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

        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
                case SDL_KEYDOWN:
                {
                    input::Key::Type key = KeyMap::SDLKeyToSparkKey(ev.key.keysym.scancode);
                    input::KeyEvent keyEvent;
                    keyEvent.key = key;
                    keyEvent.state = input::KeyState::Down;
                    inputComp->keyEvents.push_back(keyEvent);
                    inputComp->keyStates[key] = keyEvent.state;
                    break;
                }
                case SDL_KEYUP:
                {
                    input::Key::Type key = KeyMap::SDLKeyToSparkKey(ev.key.keysym.scancode);
                    input::KeyEvent keyEvent;
                    keyEvent.key = key;
                    keyEvent.state = input::KeyState::Up;
                    inputComp->keyEvents.push_back(keyEvent);
                    inputComp->keyStates[key] = keyEvent.state;
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                {
                    inputComp->mouseButtonStates[ev.button.button] = input::KeyState::Down;

                    input::MouseEvent mouseEvent;
                    mouseEvent.button = static_cast<input::MouseButton::Type>(ev.button.button);
                    mouseEvent.state = input::KeyState::Down;
                    inputComp->mouseEvents.push_back(mouseEvent);
                    break;
                }
                case SDL_MOUSEBUTTONUP:
                {
                    inputComp->mouseButtonStates[ev.button.button] = input::KeyState::Up;

                    input::MouseEvent mouseEvent;
                    mouseEvent.button = static_cast<input::MouseButton::Type>(ev.button.button);
                    mouseEvent.state = input::KeyState::Up;
                    inputComp->mouseEvents.push_back(mouseEvent);
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    inputComp->mouseX = ev.motion.x;
                    inputComp->mouseY = ev.motion.y;
                    break;
                }
                case SDL_MOUSEWHEEL:
                {
                    inputComp->mouseScrollDelta = ev.wheel.y;

                    input::MouseEvent mouseEvent;
                    mouseEvent.button = input::MouseButton::None;
                    mouseEvent.scrollDelta = inputComp->mouseScrollDelta;
                    inputComp->mouseEvents.push_back(mouseEvent);
                    break;
                }
                case SDL_WINDOWEVENT:
                    if (!m_Windows.contains(ev.window.windowID))
                    {
                        break;
                    }
                    auto* window = m_Windows.at(ev.window.windowID);
                    switch (ev.window.event)
                    {
                        case SDL_WINDOWEVENT_RESIZED:
                            window->OnResize(ev.window.data1, ev.window.data2);
                            break;
                        case SDL_WINDOWEVENT_MOVED:
                            window->OnMove(ev.window.data1, ev.window.data2);
                            break;
                        case SDL_WINDOWEVENT_CLOSE:
                            window->OnClose();
                            break;
                        default:
                            break; //TODO
                    }
                    break;
            }
        }

        auto safeCopy = m_Windows;
        for (const auto& [id, window]: safeCopy)
        {
            if (window->ShouldClose())
            {
                window->Cleanup();
                delete window;
            }
        }

        if (ShouldExit())
        {
            return;
        }

        PlatformRunLoop::Update();

        for (const auto& [id, window]: m_Windows)
        {
            window->SetCurrent();
            render::Renderer::Get()->Render(window);
            SDL_GL_SwapWindow(window->GetSDLWindow());
        }

        render::Renderer::Get()->EndFrame();
    }

    bool LinuxRunLoop::ShouldExit()
    {
        return m_ShouldExit;
    }

    void LinuxRunLoop::RegisterWindow(IWindow* window)
    {
        Window* platformWindow = dynamic_cast<Window*>(window);
        SPARK_ASSERT(platformWindow);
        SPARK_ASSERT(platformWindow->GetSDLWindow());
        uint32_t id = SDL_GetWindowID(platformWindow->GetSDLWindow());
        m_Windows[id] = platformWindow;
    }

    void LinuxRunLoop::UnregisterWindow(IWindow* window)
    {
        Window* platformWindow = dynamic_cast<Window*>(window);
        SPARK_ASSERT(platformWindow);
        SPARK_ASSERT(platformWindow->GetSDLWindow());
        uint32_t id = SDL_GetWindowID(platformWindow->GetSDLWindow());
        m_Windows.erase(id);
        if (m_Windows.empty() || window == Application::Get()->GetPrimaryWindow())
        {
            m_ShouldExit = true;
        }
    }
}
