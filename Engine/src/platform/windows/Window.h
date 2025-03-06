#pragma once
#include "platform/IWindow.h"

namespace se::windows
{
    class Window : public se::IWindow
    {
    public:
        Window(int resX, int resY);
        ~Window();

        void Cleanup() override;

        HWND GetHWND() { return m_Hwnd; }
        HDC GetHDC() { return m_Hdc; }
        HGLRC GetHGLRC() { return m_Gglrc; }
    private:
        void RegisterWindowClass(HINSTANCE instance);
        void CreateWindowsWindow(HINSTANCE instance);
        void CreateContext();

        HWND m_Hwnd = {};
        HDC m_Hdc = {};
        HGLRC m_Gglrc = {};
    };

    HGLRC wglCreateContextAttribsARB(HDC hDC, HGLRC hshareContext, const int *attribList);
}
