#include "TestApplication.h"

#include "engine/math/Mat4.h"
#include "engine/render/Material.h"
#include "engine/render/Renderer.h"
#include "engine/render/VertexBuffer.h"
#include "engine/shader/ast/Types.h"
#include "platform/IWindow.h"
#include "engine/io/VFS.h"

namespace app
{
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

    TestApplication* TestApplication::GetTestApplication()
    {
        return static_cast<TestApplication*>(Get());
    }

    void TestApplication::Init()
    {
        using namespace se;

        // would be nice if the engine could handle this.
        io::VFS::Get().Mount("../../TestProject/assets", "/source_assets");
        io::VFS::Get().Mount("../../TestProject/built", "/assets");

        Application::Init();

        m_Material1 = render::Material::CreateMaterial(
        { "/builtin_assets/shader.vert" },
        { "/builtin_assets/shader.frag",  "/builtin_assets/shader2.frag" });
        render::RenderState rs;
        rs.depthComp = render::DepthCompare::Less;
        m_Material1->SetRenderState(rs);
        m_Material1->CreatePlatformResources();

        m_Material2 = render::Material::CreateMaterial(
    { "/builtin_assets/shader.vert" },
    { "/builtin_assets/shader3.frag" });
        m_Material2->SetRenderState(rs);
        m_Material2->CreatePlatformResources();

        // Camera
        math::Mat4 proj = math::Perspective(math::Radians(45.f), (float)GetPrimaryWindow()->GetWidth() / (float)GetPrimaryWindow()->GetHeight(),
                                            .1f, 100.f);
        m_View = math::LookAt(math::Vec3(-4.f, 3.f, 4.f),
                                       math::Vec3(0.f, 0.f, 0.f),
                                       math::Vec3(0.f, 1.f, 0.f));
        math::Mat4 model = math::Translation(math::Vec3(-1.1, 0, -1.1));
        math::Mat4 mvp = proj * m_View * model;
        m_Material2->SetUniform("MVP", shader::ast::Type::Mat4, &mvp[0]);

        math::Mat4 mvp2 = proj * m_View * math::Translation(math::Vec3(1.1, 0, 1.1));
        m_Material2->SetUniform("MVP", shader::ast::Type::Mat4, &mvp2[0]);


        render::VertexStream posStream;
        posStream.type = render::VertexStreamType::Position;
        posStream.stride = 3;
        posStream.data = s_CubePositions;

        render::VertexStream colourStream;
        colourStream.type = render::VertexStreamType::Colour;
        colourStream.stride = 3;
        colourStream.data = s_CubeColours;

        m_VertBuffer = render::VertexBuffer::CreateVertexBuffer({ posStream, colourStream });
        m_VertBuffer->CreatePlatformResource();
    }

    void TestApplication::Update()
    {
        using namespace se;
        math::Mat4 proj = math::Perspective(math::Radians(45.f), (float)GetPrimaryWindow()->GetWidth() / (float)GetPrimaryWindow()->GetHeight(),.1f, 100.f);
        math::Mat4 mvp = proj * m_View * math::Translation(math::Vec3(-1.1, 0, -1.1));
        m_Material1->SetUniform("MVP", shader::ast::Type::Mat4, &mvp[0]);

        math::Mat4 mvp2 = proj * m_View * math::Translation(math::Vec3(1.1, 0, 1.1));
        m_Material2->SetUniform("MVP", shader::ast::Type::Mat4, &mvp2[0]);
    }

    void TestApplication::Render()
    {
        auto renderer = se::render::Renderer::Get();
        auto window = GetPrimaryWindow();
        renderer->Submit(window, se::render::RenderCommand([]
        {
            se::render::RenderCommand::Clear(true, true);
        }));

        renderer->Submit(window, se::render::RenderCommand([this]
        {
            se::render::RenderCommand::SubmitGeo(m_Material1, m_VertBuffer, 12 * 3);
            se::render::RenderCommand::SubmitGeo(m_Material2, m_VertBuffer, 12 * 3);
        }));
    }
}
