#include "IWindow.h"

#include "GL/glew.h"

namespace se
{
    void IWindow::OnResize(int x, int y)
    {
        m_SizeX = x;
        m_SizeY = y;

        glViewport(0, 0, x, y); //TODO this should be abstracted away
    }

    void IWindow::OnMove(int x, int y)
    {
        m_PosX = x;
        m_PosY = y;
    }
}
