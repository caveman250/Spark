#define UNICODE 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <gl/GL.h>
#include <Gl/glew.h>

#include "engine/render/RenderCommand.h"
#include "engine/render/Renderer.h"
#include "platform/IRunLoop.h"
#include "platform/IWindow.h"


int main(int argc, char *argv[])
{
    se::IWindow* window = se::IWindow::CreatePlatformWindow(1920, 1080);
    se::IWindow* window2 = se::IWindow::CreatePlatformWindow(1920, 1080);
    se::IRunLoop* runLoop = se::IRunLoop::CreatePlatformRunloop({ window, window2 });
    auto renderer = se::render::Renderer::Create();

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    while (true)
    {
        //Temp
        renderer->SubmitRenderCommand(window, se::render::RenderCommand([]()
        {
            glClear(GL_COLOR_BUFFER_BIT);
        }));

        renderer->SubmitRenderCommand(window, se::render::RenderCommand([]()
        {
            glBegin(GL_TRIANGLES);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2i(0, 1);
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex2i(-1, -1);
            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex2i(1, -1);
            glEnd();
        }));

        renderer->SubmitRenderCommand(window, se::render::RenderCommand([]()
        {
            glFlush();
        }));

        renderer->SubmitRenderCommand(window2, se::render::RenderCommand([]()
        {
            glClear(GL_COLOR_BUFFER_BIT);
        }));

        renderer->SubmitRenderCommand(window2, se::render::RenderCommand([]()
        {
            glBegin(GL_TRIANGLES);
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex2i(0, 1);
            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex2i(-1, -1);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2i(1, -1);
            glEnd();
        }));

        renderer->SubmitRenderCommand(window2, se::render::RenderCommand([]()
        {
            glFlush();
        }));

        runLoop->Tick();
    }

    delete window;
    delete runLoop;
}
