#include "spark.h"

#include "engine/math/Mat4.h"

#include "engine/render/Material.h"
#include "engine/render/Renderer.h"
#include "engine/render/VertexBuffer.h"
#include "engine/shader/ast/Types.h"

#include "engine/asset/binary/Struct.h"
#include "engine/asset/binary/Asset.h"

#include "platform/IRunLoop.h"
#include "platform/IWindow.h"

// This is just a dumping ground to test systems currently

int main(int argc, char* argv[])
{
    using namespace se;
    auto renderer = render::Renderer::Create();
    IWindow* window = IWindow::CreatePlatformWindow(1280, 720);
    IRunLoop* runLoop = IRunLoop::CreatePlatformRunloop({window});

    std::shared_ptr<render::Material> material = render::Material::CreateMaterial(
        { "../builtin_assets/shader.vert" },
        { "../builtin_assets/shader.frag",  "../builtin_assets/shader2.frag" });
    render::RenderState rs;
    rs.depthComp = render::DepthCompare::Less;
    material->SetRenderState(rs);
    material->CreatePlatformResources();

    std::shared_ptr<render::Material> material2 = render::Material::CreateMaterial(
{ "../builtin_assets/shader.vert" },
{ "../builtin_assets/shader3.frag" });
    material2->SetRenderState(rs);
    material2->CreatePlatformResources();

    // Camera
    math::Mat4 proj = math::Perspective(math::Radians(45.f), (float)window->GetWidth() / (float)window->GetHeight(),
                                        .1f, 100.f);
    math::Mat4 view = math::LookAt(math::Vec3(-4.f, 3.f, 4.f),
                                   math::Vec3(0.f, 0.f, 0.f),
                                   math::Vec3(0.f, 1.f, 0.f));
    math::Mat4 model = math::Translation(math::Vec3(-1.1, 0, -1.1));
    math::Mat4 mvp = proj * view * model;
    material->SetUniform("MVP", shader::ast::Type::Mat4, &mvp[0]);

    math::Mat4 mvp2 = proj * view * math::Translation(math::Vec3(1.1, 0, 1.1));
    material2->SetUniform("MVP", shader::ast::Type::Mat4, &mvp2[0]);

    static const std::vector<float> s_CubePositions =
    {
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
    };

    static const std::vector<float> s_CubeColours = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };

    render::VertexStream posStream;
    posStream.type = render::VertexStreamType::Position;
    posStream.stride = 3;
    posStream.data = s_CubePositions;

    render::VertexStream colourStream;
    colourStream.type = render::VertexStreamType::Colour;
    colourStream.stride = 3;
    colourStream.data = s_CubeColours;

    std::shared_ptr<render::VertexBuffer> vertBuffer = render::VertexBuffer::CreateVertexBuffer({posStream, colourStream });
    vertBuffer->CreatePlatformResource();

    while (!runLoop->ShouldExit())
    {
        renderer->SubmitRenderCommand(window, render::RenderCommand([]
        {
            render::RenderCommand::Clear(true, true);
        }));

        renderer->SubmitRenderCommand(window, render::RenderCommand([material, material2, vertBuffer]
        {
            render::RenderCommand::SubmitGeo(material, vertBuffer, 12*3);
            render::RenderCommand::SubmitGeo(material2, vertBuffer, 12*3);
        }));

        runLoop->Tick();
    }

    std::shared_ptr<asset::binary::Asset> asset = asset::binary::Asset::Create(false);
    asset::binary::StructLayout structLayout =
    {
        { asset::binary::CreateFixedString32("pos"), asset::binary::Type::Vec2 },
        { asset::binary::CreateFixedString32("colour"), asset::binary::Type::Vec3 },
    };
    auto structIndex1 = asset->AddStruct(structLayout);

    asset::binary::StructLayout structLayout2 =
    {
        {asset::binary::CreateFixedString32("pos2"),    asset::binary::Type::Float},
        {asset::binary::CreateFixedString32("colour2"), asset::binary::Type::Int8},
    };
    auto structIndex2 = asset->AddStruct(structLayout2);

    asset->SetRootStruct(structIndex1);
    auto obj = asset->GetRoot();
    obj.Set("pos", math::Vec2(4.f, 4.f));
    obj.Set("colour", math::Vec3(1.f, 0.5f, 0.2f));
    asset->Save("test.sass");

    std::shared_ptr<asset::binary::Asset> asset2 = asset::binary::Asset::Load("test.sass", false);
    auto obj2 = asset2->GetRoot();
    auto pos = obj2.Get<math::Vec2>("pos");
    auto colour = obj2.Get<math::Vec3>("colour");

    delete runLoop;
}
