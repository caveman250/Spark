#include "IWindow.h"

#include "engine/Application.h"
#include "engine/render/opengl/GL_fwd.h"

namespace se
{
    void IWindow::OnResize(int x, int y)
    {
        m_SizeX = x;
        m_SizeY = y;
        OnResized.Broadcast(m_SizeX, m_SizeY);
    }

    void IWindow::OnMove(int x, int y)
    {
        m_PosX = x;
        m_PosY = y;
        OnMoved.Broadcast(m_PosX, m_PosY);
    }
}
