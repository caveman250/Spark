#include "Window.h"
#include "platform/IWindow.h"
#include <gl/GL.h>

#include <unordered_map>

#include "engine/logging/Log.h"
#include "platform/IRunLoop.h"

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
        IRunLoop::Get()->UnregisterWindow(this);
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

    void Window::OnResize(int x, int y)
    {
        SizeX = x;
        SizeY = y;

        glViewport(0, 0, x, y);
    }

    static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        Window* window = s_WindowInstances[hWnd];

        switch (message)
        {
            case WM_PAINT:
                break;
            case WM_SIZE:
                window->OnResize(LOWORD(lParam), HIWORD(lParam));
                break;
            case WM_KEYDOWN:
                break;
            case WM_CLOSE:
                wglMakeCurrent(window->GetHDC(), NULL);
                wglDeleteContext(window->GetHGLRC());
                ReleaseDC(hWnd, window->GetHDC());
                DestroyWindow(hWnd);
                s_WindowInstances.erase(hWnd);
                delete window;
                break;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);;
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
            logging::Log::Fatal("RegisterClassExW failed: Can not register window class.");
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
                logging::Log::Fatal("CreateWindowW failed: Can not create window.");
            }
        } else
        {
            logging::Log::Fatal("AdjustWindowRect failed: Can not create window.");
        }
    }

    void Window::CreateContext()
    {
        Hdc = GetDC(Hwnd);
        if (Hdc)
        {
            int pixelFormat;
            PIXELFORMATDESCRIPTOR pixelFormatDesc;

            /* initialize bits to 0 */
            memset(&pixelFormatDesc, 0, sizeof(PIXELFORMATDESCRIPTOR));
            pixelFormatDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
            pixelFormatDesc.nVersion = 1;
            pixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
            pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
            pixelFormatDesc.cColorBits = 32;
            pixelFormatDesc.cAlphaBits = 8;
            pixelFormatDesc.cDepthBits = 24;

            pixelFormat = ChoosePixelFormat(Hdc, &pixelFormatDesc);
            if (pixelFormat)
            {
                if (SetPixelFormat(Hdc, pixelFormat, &pixelFormatDesc))
                {
                    Gglrc = wglCreateContext(Hdc);
                    if (!Gglrc)
                    {
                        logging::Log::Fatal("wglCreateContext failed: Can not create render context.");
                    }
                } else
                {
                    logging::Log::Fatal("SetPixelFormat failed: Can not create render context.");
                }
            } else
            {
                logging::Log::Fatal("ChoosePixelFormat failed: Can not create render context.");
            }
        } else
        {
            logging::Log::Fatal("GetDC failed: Can not create device context.");
        }
    }
}
