#pragma once

#include "engine/ecs/Signal.h"

namespace se
{
    class IWindow
    {
    public:
        static IWindow* CreatePlatformWindow(int posX, int posY, int resX, int resY);

        IWindow(int posX, int posY, int resX, int resY) : m_PosX(posX) , m_PosY(posY), m_SizeX(resX), m_SizeY(resY) {}
        virtual ~IWindow() = default;
        virtual void Cleanup() = 0;

        int GetWidth() const { return static_cast<int>(m_SizeX * m_ContentScale); }
        int GetHeight() const { return static_cast<int>(m_SizeY * m_ContentScale); }
        int GetPosX() const { return m_PosX; }
        int GetPosY() const { return m_PosY; }
        float GetContentScale() const { return m_ContentScale; }
        float GetLastContentScale() const { return m_LastContentScale; }
        void SetLastContentScale(float scale) { m_LastContentScale = scale; }

        void OnResize(int x, int y);
        void OnMove(int x, int y);

        void OnClose() { m_ShouldClose = true; }
        bool ShouldClose() const { return m_ShouldClose; }

        ecs::Signal<int, int> OnResized = {};
        ecs::Signal<int, int> OnMoved = {};

    protected:
        int m_PosX = {};
        int m_PosY = {};
        int m_SizeX = {};
        int m_SizeY = {};
        float m_ContentScale = 1.f;
        float m_LastContentScale = 1.f;

        bool m_ShouldClose = false;
    };
}