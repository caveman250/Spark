#pragma once

#include <engine/input/InputComponent.h>
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
        void SetCursor(HCURSOR cursor);
        HCURSOR GetCursor() const { return m_Cursor; }
        input::InputComponent& GetTempInputComponent();

        void OnContentScaleChanged(float contentScale);

    private:
        void RegisterWindowClass(HINSTANCE instance);
        void CreateWindowsWindow(HINSTANCE instance);
        void CreateContext();
        void CalcContentScale();

        HWND m_Hwnd = {};
        HDC m_Hdc = {};
        HGLRC m_Gglrc = {};
        HCURSOR m_Cursor = {};
        input::InputComponent m_TempInputComponent;
    };

    HGLRC wglCreateContextAttribsARB(HDC hDC, HGLRC hshareContext, const int *attribList);
}
