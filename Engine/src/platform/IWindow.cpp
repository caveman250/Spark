#include "IWindow.h"

#include "engine/render/opengl/GL_fwd.h"

namespace se
{
    void IWindow::OnResize(int x, int y)
    {
        m_SizeX = x;
        m_SizeY = y;

#if OPENGL_RENDERER
        glViewport(0, 0, x, y); //TODO this should be abstracted away
#endif
    }

    void IWindow::OnMove(int x, int y)
    {
        m_PosX = x;
        m_PosY = y;
    }
}
