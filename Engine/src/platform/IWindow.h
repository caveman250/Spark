#pragma once

namespace se
{
    class IWindow
    {
    public:
        static IWindow* CreatePlatformWindow(int resX, int resY);

        IWindow(int resX, int resY) : m_PosX(0) , m_PosY(0), m_SizeX(resX), m_SizeY(resY) {}
        virtual ~IWindow() = default;
        virtual void Cleanup() = 0;

        int GetWidth() { return m_SizeX; }
        int GetHeight() { return m_SizeY; }
        int GetPosX() { return m_PosX; }
        int GetPosY() { return m_PosY; }

        void OnResize(int x, int y);
        void OnMove(int x, int y);

        void OnClose() { m_ShouldClose = true; }
        bool ShouldClose() const { return m_ShouldClose; }

    protected:
        int m_PosX = {};
        int m_PosY = {};
        int m_SizeX = {};
        int m_SizeY = {};

        bool m_ShouldClose = false;
    };
}