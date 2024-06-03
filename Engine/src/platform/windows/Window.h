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
        int GetPosX() override;
        int GetPosY() override;

        void OnResize(int x, int y);
        void OnMove(int x, int y);

        HWND GetHWND() { return Hwnd; }
        HDC GetHDC() { return Hdc; }
        HGLRC GetHGLRC() { return Gglrc; }
    private:
        void RegisterWindowClass(HINSTANCE instance);
        void CreateWindowsWindow(HINSTANCE instance);
        void CreateContext();

        uint32_t PosX = 0;
        uint32_t PosY = 0;

        HWND Hwnd;
        HDC Hdc;
        HGLRC Gglrc;
        int SizeX, SizeY;
    };

    HGLRC wglCreateContextAttribsARB(HDC hDC, HGLRC hshareContext, const int *attribList);
}
