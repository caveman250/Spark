#include "Material.h"

#if OPENGL_RENDERER

#include "engine/asset/shader/compiler/ShaderCompiler.h"
#include "engine/asset/shader/ast/Types.h"
#include "engine/asset/shader/ast/TypeUtil.h"
#include "TextureResource.h"

namespace se::render
{
    std::shared_ptr<Material> Material::CreateMaterial(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                                                        const std::vector<std::shared_ptr<asset::Shader>>& fragShaders)
    {
        return std::make_shared<opengl::Material>(vertShaders, fragShaders);
    }
}

namespace se::render::opengl
{
    Material::Material(const std::vector<std::shared_ptr<asset::Shader>>& vertShaders,
                        const std::vector<std::shared_ptr<asset::Shader>>& fragShaders)
        : render::Material(vertShaders, fragShaders)
    {
    }

    void Material::Bind(const render::VertexBuffer& vb)
    {
        render::Material::Bind(vb);
        // nothing to do
    }

    void Material::CreatePlatformResources(const render::VertexBuffer& vb)
    {
        SPARK_ASSERT(m_VertexShader == GL_INVALID_VALUE && m_FragmentShader == GL_INVALID_VALUE);

        asset::shader::ast::ShaderCompileContext context;

        std::optional<std::string> vert = asset::shader::ShaderCompiler::GeneratePlatformShader(m_VertShaders, m_ShaderSettings, vb, context);
        std::optional<std::string> frag = asset::shader::ShaderCompiler::GeneratePlatformShader(m_FragShaders, m_ShaderSettings, vb, context);

        if (!vert.has_value() || !frag.has_value())
        {
            return;
        }
        else
        {
            //debug::Log::Info("Result Vert Shader:\n {}", vert.value());
            debug::Log::Info("Result Frag Shader:\n {}", frag.value());
        }

        // Create the shaders
        m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
        GL_CHECK_ERROR()
        m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        GL_CHECK_ERROR()

        char const *VertexSourcePointer = vert.value().c_str();
        glShaderSource(m_VertexShader, 1, &VertexSourcePointer, NULL);
        GL_CHECK_ERROR()
        glCompileShader(m_VertexShader);
        GL_CHECK_ERROR()

        GLint Result = GL_FALSE;
        int InfoLogLength;
        glGetShaderiv(m_VertexShader, GL_COMPILE_STATUS, &Result);
        GL_CHECK_ERROR()
        glGetShaderiv(m_VertexShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
        GL_CHECK_ERROR()
        if (InfoLogLength > 0)
        {
            std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(m_VertexShader, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            GL_CHECK_ERROR()
            printf("%s\n", &VertexShaderErrorMessage[0]);
        }

        // Compile Fragment Shader
        char const *FragmentSourcePointer = frag.value().c_str();
        glShaderSource(m_FragmentShader, 1, &FragmentSourcePointer, NULL);
        GL_CHECK_ERROR()
        glCompileShader(m_FragmentShader);
        GL_CHECK_ERROR()

        glGetShaderiv(m_FragmentShader, GL_COMPILE_STATUS, &Result);
        GL_CHECK_ERROR()
        glGetShaderiv(m_FragmentShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
        GL_CHECK_ERROR()
        if (InfoLogLength > 0)
        {
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(m_FragmentShader, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
            GL_CHECK_ERROR()
            debug::Log::Error("{0}", &FragmentShaderErrorMessage[0]);
        }

        render::Material::CreatePlatformResources(vb);
    }

    void Material::DestroyPlatformResources()
    {
        if (m_VertexShader != GL_INVALID_VALUE)
        {
            glDeleteShader(m_VertexShader);
            GL_CHECK_ERROR()
            m_VertexShader = GL_INVALID_VALUE;
        }

        if (m_FragmentShader != GL_INVALID_VALUE)
        {
            glDeleteShader(m_FragmentShader);
            GL_CHECK_ERROR()
            m_FragmentShader = GL_INVALID_VALUE;
        }

        render::Material::DestroyPlatformResources();
    }

    void Material::ApplyDepthStencil(DepthCompare comp, StencilFunc src, uint32_t writeMask, uint32_t readMask)
    {
        if (comp == DepthCompare::None)
        {
            glDisable(GL_DEPTH_TEST);
            GL_CHECK_ERROR()
        }
        else
        {
            glEnable(GL_DEPTH_TEST);
            GL_CHECK_ERROR()
        }

        if (comp != DepthCompare::None)
        {
            switch (comp)
            {
                case DepthCompare::Less:
                    glDepthFunc(GL_LESS);
                    GL_CHECK_ERROR()
                    break;
                case DepthCompare::LessEqual:
                    glDepthFunc(GL_LEQUAL);
                    GL_CHECK_ERROR()
                    break;
                case DepthCompare::Equal:
                    glDepthFunc(GL_EQUAL);
                    GL_CHECK_ERROR()
                    break;
                case DepthCompare::Greater:
                    glDepthFunc(GL_GREATER);
                    GL_CHECK_ERROR()
                    break;
                case DepthCompare::GreaterEqual:
                    glDepthFunc(GL_GEQUAL);
                    GL_CHECK_ERROR()
                    break;
                default:
                    SPARK_ASSERT(false);
                    break;
            }

            GL_CHECK_ERROR()
        }

        if (!writeMask && src == StencilFunc::None)
        {
            glDisable(GL_STENCIL_TEST);
            GL_CHECK_ERROR()
            return;
        }

        glEnable(GL_STENCIL_TEST);
        GL_CHECK_ERROR()
        glStencilMask(writeMask);
        GL_CHECK_ERROR()
        if (src != StencilFunc::None)
        {
            switch (src)
            {
                case StencilFunc::Less:
                    glStencilFunc(GL_LESS, 1, readMask);
                    GL_CHECK_ERROR()
                    break;
                case StencilFunc::LessEqual:
                    glStencilFunc(GL_LEQUAL, 1, readMask);
                    GL_CHECK_ERROR()
                    break;
                case StencilFunc::Equal:
                    glStencilFunc(GL_EQUAL, 1, readMask);
                    GL_CHECK_ERROR()
                    break;
                case StencilFunc::Greater:
                    glStencilFunc(GL_GREATER, 1, readMask);
                    GL_CHECK_ERROR()
                    break;
                case StencilFunc::GreaterEqual:
                    glStencilFunc(GL_GEQUAL, 1, readMask);
                    GL_CHECK_ERROR()
                    break;
                case StencilFunc::None:
                    glStencilFunc(GL_NEVER, 0, 0);
                    GL_CHECK_ERROR()
                    break;
            }
        }
    }

    GLuint BlendModeToGLBlendMode(BlendMode blendMode)
    {
        switch (blendMode)
        {
            case BlendMode::Zero:
                return GL_ZERO;
            case BlendMode::One:
                return GL_ONE;
            case BlendMode::SrcColor:
                return GL_SRC_COLOR;
            case BlendMode::OneMinusSrcColor:
                return GL_ONE_MINUS_SRC_COLOR;
            case BlendMode::DstColor:
                return GL_DST_COLOR;
            case BlendMode::OneMinusDstColor:
                return GL_ONE_MINUS_DST_COLOR;
            case BlendMode::SrcAlpha:
                return GL_SRC_ALPHA;
            case BlendMode::OneMinusSrcAlpha:
                return GL_ONE_MINUS_SRC_ALPHA;
            case BlendMode::DstAlpha:
                return GL_DST_ALPHA;
            case BlendMode::OneMinusDstAlpha:
                return GL_ONE_MINUS_DST_ALPHA;
            default:
                SPARK_ASSERT(false);
                return GL_ZERO;
        }
    }

    void Material::ApplyBlendMode(BlendMode src, BlendMode dst)
    {
        if (src == BlendMode::None || dst == BlendMode::None)
        {
            glDisable(GL_BLEND);
            GL_CHECK_ERROR()
            return;
        }
        else
        {
            glEnable(GL_BLEND);
            GL_CHECK_ERROR()
        }

        glBlendFunc(BlendModeToGLBlendMode(src), BlendModeToGLBlendMode(dst));
        GL_CHECK_ERROR()
    }
}

#endif