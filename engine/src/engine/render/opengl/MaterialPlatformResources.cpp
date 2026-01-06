#include "MaterialPlatformResources.h"

#if OPENGL_RENDERER

namespace se::render
{
	std::shared_ptr<MaterialPlatformResources> Material::CreateMaterialPlatformResources()
	{
		return std::make_shared<opengl::MaterialPlatformResources>();
	}
}

namespace se::render::opengl
{
    void MaterialPlatformResources::Bind()
    {

    }

    void MaterialPlatformResources::CreatePlatformResources(const std::string& vert,
                                                            const std::string& frag,
                                                            const RenderState& rs)
    {
        m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
        GL_CHECK_ERROR()
        m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        GL_CHECK_ERROR()

        char const *VertexSourcePointer = vert.c_str();
        glShaderSource(m_VertexShader, 1, &VertexSourcePointer, NULL);
        GL_CHECK_ERROR()
        glCompileShader(m_VertexShader);
        GL_CHECK_ERROR()

        GLint result = GL_FALSE;
        int InfoLogLength;
        glGetShaderiv(m_VertexShader, GL_COMPILE_STATUS, &result);
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
        char const *FragmentSourcePointer = frag.c_str();
        glShaderSource(m_FragmentShader, 1, &FragmentSourcePointer, NULL);
        GL_CHECK_ERROR()
        glCompileShader(m_FragmentShader);
        GL_CHECK_ERROR()

        glGetShaderiv(m_FragmentShader, GL_COMPILE_STATUS, &result);
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
    }

    void MaterialPlatformResources::DestroyPlatformResources()
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
    }

    void MaterialPlatformResources::ApplyDepthStencil(const RenderState& rs)
    {
         if (rs.depthComp == DepthCompare::None)
        {
            glDisable(GL_DEPTH_TEST);
            GL_CHECK_ERROR()
        }
        else
        {
            glEnable(GL_DEPTH_TEST);
            GL_CHECK_ERROR()
        }

        if (rs.depthComp != DepthCompare::None)
        {
            switch (rs.depthComp)
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

        if (!rs.stencilWriteMask && rs.stencilFunc == StencilFunc::None)
        {
            glDisable(GL_STENCIL_TEST);
            GL_CHECK_ERROR()
            return;
        }

        glEnable(GL_STENCIL_TEST);
        GL_CHECK_ERROR()
        glStencilMask(rs.stencilWriteMask);
        GL_CHECK_ERROR()
        if (rs.stencilFunc != StencilFunc::None)
        {
            switch (rs.stencilFunc)
            {
                case StencilFunc::Less:
                    glStencilFunc(GL_LESS, 1, rs.stencilReadMask);
                    GL_CHECK_ERROR()
                    break;
                case StencilFunc::LessEqual:
                    glStencilFunc(GL_LEQUAL, 1, rs.stencilReadMask);
                    GL_CHECK_ERROR()
                    break;
                case StencilFunc::Equal:
                    glStencilFunc(GL_EQUAL, 1, rs.stencilReadMask);
                    GL_CHECK_ERROR()
                    break;
                case StencilFunc::Greater:
                    glStencilFunc(GL_GREATER, 1, rs.stencilReadMask);
                    GL_CHECK_ERROR()
                    break;
                case StencilFunc::GreaterEqual:
                    glStencilFunc(GL_GEQUAL, 1, rs.stencilReadMask);
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

    void MaterialPlatformResources::ApplyBlendMode(BlendMode src,
        BlendMode dest)
    {
        if (src == BlendMode::None || dest == BlendMode::None)
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

        glBlendFunc(BlendModeToGLBlendMode(src), BlendModeToGLBlendMode(dest));
        GL_CHECK_ERROR()
    }
}

#endif
