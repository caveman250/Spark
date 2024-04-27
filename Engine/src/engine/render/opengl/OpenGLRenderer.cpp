#include "OpenGLRenderer.h"
#include "GL_fwd.h"

#include "engine/render/RenderCommand.h"

namespace se::render::opengl
{
    OpenGLRenderer::OpenGLRenderer()
    {

    }

    static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    void OpenGLRenderer::Init()
    {
        auto hInstance = GetModuleHandle(NULL);
        HWND hwnd;
        HDC hdc;

        //---- fake Window
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof( WNDCLASSEX );
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcex.lpfnWndProc = wndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon( NULL, IDI_APPLICATION );
        wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
        wcex.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = "coco";
        wcex.hIconSm = NULL;

        if( !RegisterClassEx( &wcex ) )
        {
            debug::Log::Fatal("Failed to register fake window class");
        }
        hwnd = CreateWindow(
            "coco",
            "dddd",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            500, 500,
            NULL,
            NULL,
            hInstance,
            NULL
        );

        hdc = GetDC( hwnd );

        PIXELFORMATDESCRIPTOR& pfd = GetPixelFormatDecriptor();
        memset( &pfd, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
        pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
        pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 32;
        pfd.iLayerType = PFD_MAIN_PLANE;

        int nPixelFormat = ChoosePixelFormat( hdc, &pfd );

        SetPixelFormat( hdc, nPixelFormat, &pfd );

        HGLRC hrc = wglCreateContext( hdc );

        wglMakeCurrent( hdc, hrc );

        glewExperimental = true;
        if (glewInit() != GLEW_OK)
        {
            debug::Log::Fatal("Failed to initialize GLEW");
        }

        wglMakeCurrent( NULL, NULL );
        wglDeleteContext( hrc );
        ReleaseDC( hwnd, hdc );
        DestroyWindow( hwnd );
    }

    void OpenGLRenderer::Render(IWindow* window)
    {
        ExecuteDrawCommands(window);
        glFlush();
    }

    void OpenGLRenderer::ApplyDepthCompare(DepthCompare comp)
    {
        if (comp == DepthCompare::None)
        {
            glDisable(GL_DEPTH_TEST);
        }
        else
        {
            glEnable(GL_DEPTH_TEST);
        }

        GL_CHECK_ERROR()

        switch (comp)
        {
        case DepthCompare::Less:
            glDepthFunc(GL_LESS);
            break;
        case DepthCompare::LessEqual:
            glDepthFunc(GL_LESS);
            break;
        case DepthCompare::Equal:
            glDepthFunc(GL_LESS);
            break;
        case DepthCompare::Greater:
            glDepthFunc(GL_LESS);
            break;
        case DepthCompare::GreaterEqual:
            glDepthFunc(GL_LESS);
            break;
        }

        GL_CHECK_ERROR()
    }
}
