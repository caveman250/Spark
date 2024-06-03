#pragma once

namespace se
{
    class IWindow
    {
    public:
        static IWindow* CreatePlatformWindow(int resX, int resY);
        virtual void SetCurrent() = 0;
        virtual int GetWidth() = 0;
        virtual int GetHeight() = 0;
        virtual int GetPosX() = 0;
        virtual int GetPosY() = 0;
    };
}