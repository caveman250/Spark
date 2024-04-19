#include "spark.h"
#include "engine/math/Mat4.h"

#include "engine/render/Material.h"
#include "engine/render/opengl/GL_fwd.h"

#include "engine/render/Renderer.h"
#include "engine/render/VertexBuffer.h"
#include "engine/render/opengl/Material.h"
#include "engine/shader/ast/Types.h"
#include "platform/IRunLoop.h"
#include "platform/IWindow.h"

int main(int argc, char* argv[])
{
    using namespace se;
    auto renderer = render::Renderer::Create();
    IWindow* window = IWindow::CreatePlatformWindow(1280, 720);
    IRunLoop* runLoop = IRunLoop::CreatePlatformRunloop({window});

    std::shared_ptr<render::Material> material = render::Material::CreateMaterial(
        "../builtin_assets/shader.vert", "../builtin_assets/shader.frag");
    material->CreatePlatformResources();

    // Camera
    math::Mat4 proj = math::Perspective(math::Radians(45.f), (float)window->GetWidth() / (float)window->GetHeight(), .1f, 100.f);
    math::Mat4 view = math::LookAt(math::Vec3(4.f, 3.f, 3.f),
                              math::Vec3(0.f, 0.f, 0.f),
                              math::Vec3(0.f, 1.f, 0.f));
    math::Mat4 model = math::Mat4(1.f);
    math::Mat4 mvp = proj * view * model;
    material->SetUniform("MVP", shader::ast::Type::Mat4, &mvp[0]);

    // Triangle
    GLuint VertexArrayID = {};
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    static const std::vector<math::Vec3> s_VertBufferData =
    {
        math::Vec3{-1.0f, -1.0f, 0.0f},
        math::Vec3{1.0f, -1.0f, 0.0f},
        math::Vec3{0.0f, 1.0f, 0.0f}
    };

    std::shared_ptr<render::VertexBuffer> vertBuffer = render::VertexBuffer::CreateVertexBuffer(s_VertBufferData);
    vertBuffer->CreatePlatformResource();

    while (!runLoop->ShouldExit())
    {
        renderer->SubmitRenderCommand(window, render::RenderCommand([]
        {
            render::RenderCommand::Clear();
        }));

        renderer->SubmitRenderCommand(window, render::RenderCommand([material, vertBuffer]
        {
            render::RenderCommand::SubmitGeo(material, vertBuffer, 3);
        }));

        runLoop->Tick();
    }

    delete runLoop;
}
