#include "LinuxRunLoop.h"

#include "KeyMap.h"
#include "Window.h"
#include "engine/Application.h"
#include "engine/input/InputComponent.h"
#include "engine/input/Key.h"
#include "engine/render/Renderer.h"

namespace se
{
    PlatformRunLoop* PlatformRunLoopstatic_cast<int>(::s_Instance = nullptr;

    PlatformRunLoop* PlatformRunLoop::CreatePlatformRunloop()
    {
        s_Instance = new linux::LinuxRunLoop();
        return s_Instance;
    }
}

namespace se::linux
{
    LinuxRunLoop::LinuxRunLoop()
    {
    }

    void LinuxRunLoop::Run()
    {
        Init();
        while (!ShouldExit())
        {
            Update();
            PROFILE_BEGIN_FRAME()
            PROFILE_BEGIN_THREAD()
        }
    }

    void LinuxRunLoop::Update()
    {
        auto app = Application::Get();
        auto inputComp = app->GetWorld()->GetSingletonComponent<input::InputComponent>();
        inputComp->mouseEvents.clear();
        inputComp->keyEvents.clear();

        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
                case SDL_KEYDOWN:
                {
                    input::Key key = KeyMap::SDLKeyToSparkKey(ev.key.keysym.scancode);
                    input::KeyEvent keyEvent;
                    keyEvent.key = key;
                    keyEvent.state = input::KeyState::Down;
                    inputComp->keyEvents.push_back(keyEvent);
                    inputComp->keyStates[static_cast<int>(key)] = keyEvent.state;
                    break;
                }
                case SDL_KEYUP:
                {
                    input::Key key = KeyMap::SDLKeyToSparkKey(ev.key.keysym.scancode);
                    input::KeyEvent keyEvent;
                    keyEvent.key = key;
                    keyEvent.state = input::KeyState::Up;
                    inputComp->keyEvents.push_back(keyEvent);
                    inputComp->keyStates[static_cast<int>(key)] = keyEvent.state;
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                {
                    inputComp->mouseButtonStates[ev.button.button] = input::KeyState::Down;

                    input::MouseEvent mouseEvent;
                    mouseEvent.button = static_cast<input::MouseButton>(ev.button.button);
                    mouseEvent.state = input::KeyState::Down;
                    inputComp->mouseEvents.push_back(mouseEvent);
                    break;
                }
                case SDL_MOUSEBUTTONUP:
                {
                    inputComp->mouseButtonStates[ev.button.button] = input::KeyState::Up;

                    input::MouseEvent mouseEvent;
                    mouseEvent.button = static_cast<input::MouseButton>(ev.button.button);
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
                    inputComp->mouseScrollDelta = ev.wheel.y * 50;

                    input::MouseEvent mouseEvent;
                    mouseEvent.button = input::MouseButton::None;
                    mouseEvent.scrollDelta = inputComp->mouseScrollDelta;
                    inputComp->mouseEvents.push_back(mouseEvent);
                    break;
                }
                case SDL_WINDOWEVENT:
                    switch (ev.window.event)
                    {
                        case SDL_WINDOWEVENT_RESIZED:
                            m_Window->OnResize(ev.window.data1, ev.window.data2);
                            break;
                        case SDL_WINDOWEVENT_MOVED:
                            m_Window->OnMove(ev.window.data1, ev.window.data2);
                            break;
                        case SDL_WINDOWEVENT_CLOSE:
                            m_Window->OnClose();
                            break;
                        default:
                            break;
                    }
                    break;
            }
        }

        if (m_Window->ShouldClose())
        {
            m_Window->Cleanup();
            delete m_Window;
            m_ShouldExit = true;
        }

        if (ShouldExit())
        {
            return;
        }

        PlatformRunLoop::Update();

        render::Renderer::Get<render::Renderer>()->Render();
        SDL_GL_SwapWindow(static_cast<Window*>(m_Window)->GetSDLWindow());

        render::Renderer::Get<render::Renderer>()->EndFrame();
    }

    bool LinuxRunLoop::ShouldExit()
    {
        return m_ShouldExit;
    }
}
