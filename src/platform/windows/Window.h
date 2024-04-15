#pragma once
#include "platform/IWindow.h"

namespace se::windows
{
    class Window : public se::IWindow
    {
    public:
        Window(int resX, int resY);
        ~Window();

        void SetCurrent() override;
        int GetWidth() override;
        int GetHeight() override;

        void OnResize(int x, int y);

        HWND GetHWND() { return Hwnd; }
        HDC GetHDC() { return Hdc; }
        HGLRC GetHGLRC() { return Gglrc; }
    private:
        void RegisterWindowClass(HINSTANCE instance);
        void CreateWindowsWindow(HINSTANCE instance);
        void CreateContext();

        HWND Hwnd;
        HDC Hdc;
        HGLRC Gglrc;
        int SizeX, SizeY;
    };
}
