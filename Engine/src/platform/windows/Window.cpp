#include "Window.h"
#include "platform/IWindow.h"
#include <engine/render/opengl/GL_fwd.h>

#include <unordered_map>

#include "KeyMap.h"
#include "engine/Application.h"
#include "engine/input/InputComponent.h"
#include "engine/input/Key.h"
#include "engine/render/Renderer.h"
#include "engine/render/opengl/OpenGLRenderer.h"
#include "platform/PlatformRunLoop.h"

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

namespace se
{
    IWindow* IWindow::CreatePlatformWindow(int resX, int resY)
    {
        return new windows::Window(resX, resY);
    }
}

namespace se::windows
{
    static std::unordered_map<HWND, Window*> s_WindowInstances;

    Window::Window(int sizeX, int sizeY)
        : SizeX(sizeX)
          , SizeY(sizeY)
    {
        auto hInstance = GetModuleHandle(NULL);

        static bool classRegistered = false;
        if (!classRegistered)
        {
            RegisterWindowClass(hInstance);
            classRegistered = true;
        }
        CreateWindowsWindow(hInstance);

        BOOL value = TRUE;
        ::DwmSetWindowAttribute(GetHWND(), DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));

        s_WindowInstances[GetHWND()] = this;
        CreateContext();

        SetCurrent();
        ShowWindow(Hwnd, SW_NORMAL);
        UpdateWindow(Hwnd);
    }

    Window::~Window()
    {
        PlatformRunLoop::Get()->UnregisterWindow(this);
    }

    void Window::SetCurrent()
    {
        wglMakeCurrent(Hdc, Gglrc);
    }

    int Window::GetWidth()
    {
        return SizeX;
    }

    int Window::GetHeight()
    {
        return SizeY;
    }

    int Window::GetPosX()
    {
        return PosX;
    }

    int Window::GetPosY()
    {
        return PosY;
    }

    void Window::OnResize(int x, int y)
    {
        SizeX = x;
        SizeY = y;

        glViewport(0, 0, x, y);
    }

    void Window::OnMove(int x, int y)
    {
        PosX = x;
        PosY = y;
    }

    static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        Window* window = s_WindowInstances[hWnd];

        switch (message)
        {
            case WM_PAINT:
            {
                break;
            }
            case WM_SIZE:
            {
                window->OnResize(LOWORD(lParam), HIWORD(lParam));
                SetCursorPos(window->GetPosX() + window->GetWidth() / 2.f, window->GetPosY() + window->GetHeight() / 2.f);
                break;
            }
            case WM_CLOSE:
            {
                wglMakeCurrent(window->GetHDC(), NULL);
                wglDeleteContext(window->GetHGLRC());
                ReleaseDC(hWnd, window->GetHDC());
                DestroyWindow(hWnd);
                s_WindowInstances.erase(hWnd);
                delete window;
                break;
            }
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                uint32_t scanCode = (HIWORD(lParam) & (KF_EXTENDED | 0xff));
                input::Key::Type key = KeyMap::WindowsKeyToSparkKey(scanCode);
                auto inputComp = Application::Get()->GetWorld()->GetSingletonComponent<input::InputComponent>();
                auto keyEvent = input::KeyEvent
                {
                    .key = key,
                    .state = message == WM_KEYDOWN ? input::KeyState::Down : input::KeyState::Up
                };
                inputComp->keyEvents.push_back(keyEvent);
                inputComp->keyStates[key] = keyEvent.state;
                break;
            }
            case WM_MOVE:
            {
                window->OnMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                SetCursorPos(window->GetPosX() + window->GetWidth() / 2.f, window->GetPosY() + window->GetHeight() / 2.f);
                break;
            }
            case WM_MOUSEMOVE:
            {
                auto app = Application::Get();
                auto inputComp = app->GetWorld()->GetSingletonComponent<input::InputComponent>();
                inputComp->mouseX = GET_X_LPARAM(lParam);
                inputComp->mouseY = GET_Y_LPARAM(lParam);
                SetCursorPos(window->GetPosX() + window->GetWidth() / 2.f, window->GetPosY() + window->GetHeight() / 2.f);
                break;
            }
        }
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }


    void Window::RegisterWindowClass(HINSTANCE instance)
    {
        WNDCLASSEXW wcex;
        memset(&wcex, 0, sizeof(wcex));
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wcex.lpfnWndProc = (WNDPROC) wndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = instance;
        wcex.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = L"SparkApplication";
        wcex.hIconSm = NULL;

        if (!RegisterClassExW(&wcex))
        {
            debug::Log::Fatal("RegisterClassExW failed: Can not register window class.");
        }
    }


    void Window::CreateWindowsWindow(HINSTANCE instance)
    {
        DWORD style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        RECT rect = {0, 0, SizeX, SizeY};

        if (AdjustWindowRect(&rect, style, false))
        {
            LPCWSTR title = L"Spark";
            LPCWSTR windowClass = L"SparkApplication";
            Hwnd = CreateWindowW(windowClass, title, style, 0, 0, SizeX, SizeY, nullptr, nullptr, instance, nullptr);
            if (!Hwnd)
            {
                debug::Log::Fatal("CreateWindowW failed: Can not create window.");
            }
        } else
        {
            debug::Log::Fatal("AdjustWindowRect failed: Can not create window.");
        }
    }

    void Window::CreateContext()
    {
        Hdc = GetDC(Hwnd);
        if (Hdc)
        {
            auto renderer = static_cast<render::opengl::OpenGLRenderer*>(render::Renderer::Get());
            PIXELFORMATDESCRIPTOR& pfd = renderer->GetPixelFormatDecriptor();
            memset( &pfd, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
            pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
            pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
            pfd.iPixelType = PFD_TYPE_RGBA;
            pfd.cColorBits = 32;
            pfd.cDepthBits = 32;
            pfd.iLayerType = PFD_MAIN_PLANE;

            int nPixelFormat = ChoosePixelFormat( Hdc, &pfd );

            const int iPixelFormatAttribList[] = {
                WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
                WGL_COLOR_BITS_ARB, 32,
                WGL_DEPTH_BITS_ARB, 24,
                WGL_STENCIL_BITS_ARB, 8,
                0 // End of attributes list
            };
            int attributes[] = {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 3
                , WGL_CONTEXT_MINOR_VERSION_ARB, 2
                , WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB
                , 0
            };

            nPixelFormat = 0;
            UINT iNumFormats = 0;

            wglChoosePixelFormatARB( Hdc, iPixelFormatAttribList, NULL, 1, &nPixelFormat, (UINT*)&iNumFormats );

            SetPixelFormat( Hdc, nPixelFormat, &pfd );

            Gglrc = wglCreateContextAttribsARB( Hdc, 0, attributes );
        } else
        {
            debug::Log::Fatal("GetDC failed: Can not create device context.");
        }
    }
}
