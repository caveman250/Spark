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
        void OnClose();
        bool ShouldClose();
        void Cleanup();

        HWND GetHWND() { return m_Hwnd; }
        HDC GetHDC() { return m_Hdc; }
        HGLRC GetHGLRC() { return m_Gglrc; }
    private:
        void RegisterWindowClass(HINSTANCE instance);
        void CreateWindowsWindow(HINSTANCE instance);
        void CreateContext();

        uint32_t m_PosX = 0;
        uint32_t m_PosY = 0;
        int m_SizeX = 0;
        int m_SizeY = 0;

        bool m_ShouldClose = false;

        HWND m_Hwnd = {};
        HDC m_Hdc = {};
        HGLRC m_Gglrc = {};
    };

    HGLRC wglCreateContextAttribsARB(HDC hDC, HGLRC hshareContext, const int *attribList);
}
