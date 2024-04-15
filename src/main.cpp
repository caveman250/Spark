#include "spark.h"

#include "engine/render/Material.h"
#include "engine/render/opengl/GL_fwd.h"

#include "engine/render/Renderer.h"
#include "engine/render/VertexBuffer.h"
#include "platform/IRunLoop.h"
#include "platform/IWindow.h"

int main(int argc, char *argv[])
{
    se::IWindow* window = se::IWindow::CreatePlatformWindow(1280, 720);
    se::IRunLoop* runLoop = se::IRunLoop::CreatePlatformRunloop({ window });
    auto renderer = se::render::Renderer::Create();

    std::shared_ptr<se::render::Material> material = se::render::Material::CreateMaterial(
        "../builtin_assets/shader.vert", "../builtin_assets/shader.frag");
    material->CreatePlatformResources();

    GLuint VertexArrayID = {};
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    static const std::vector<se::math::Vec3> s_VertBufferData =
    {
        se::math::Vec3{-1.0f, -1.0f, 0.0f},
        se::math::Vec3{1.0f, -1.0f, 0.0f},
        se::math::Vec3{0.0f, 1.0f, 0.0f}
    };

    std::shared_ptr<se::render::VertexBuffer> vertBuffer = se::render::VertexBuffer::CreateVertexBuffer(s_VertBufferData);
    vertBuffer->CreatePlatformResource();

    while (!runLoop->ShouldExit())
    {
        renderer->SubmitRenderCommand(window, se::render::RenderCommand([]
        {
            se::render::RenderCommand::Clear();
        }));

        renderer->SubmitRenderCommand(window, se::render::RenderCommand([material, vertBuffer]
        {
            se::render::RenderCommand::SubmitGeo(material, vertBuffer, 3);
        }));

        runLoop->Tick();
    }

    delete runLoop;
}
