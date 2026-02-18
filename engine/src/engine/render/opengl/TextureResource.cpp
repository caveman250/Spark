#include "engine/render/TextureResource.h"
#include "engine/render/render_fwd.h"
#if OPENGL_RENDERER
#include "easy/profiler.h"
#include "TextureResource.h"
#include "engine/asset/texture/Mipmap.h"
#include "engine/asset/texture/Format.h"

namespace se::render
{
    std::shared_ptr<TextureResource> TextureResource::Create(const asset::Texture &texture)
    {
        return std::make_shared<opengl::TextureResource>(texture);
    }
}

namespace se::render::opengl
{
    void TextureResource::CreatePlatformResources()
    {
        EASY_FUNCTION()

        render::TextureResource::CreatePlatformResources();
        EASY_BLOCK("glGenTextures")
        glGenTextures(1, &m_ID);
        GL_CHECK_ERROR()
        glBindTexture(GL_TEXTURE_2D, m_ID);
        GL_CHECK_ERROR()
        EASY_END_BLOCK

        if (!m_MipData.empty())
        {
            int level = 0;
            for (const auto& mip : m_MipData)
            {
                if (!IsCompressedFormat(m_Format))
                {
                    EASY_BLOCK("glTexImage2D")
                    glTexImage2D(GL_TEXTURE_2D,
                                           level,
                                           TextureFormatToGLInternalFormat(m_Format),
                                           mip.m_SizeX,
                                           mip.m_SizeY,
                                           0,
                                           TextureFormatToGLFormat(m_Format),
                                           GL_UNSIGNED_BYTE,
                                           mip.m_Data.GetData());
                    GL_CHECK_ERROR()
                }
                else
                {
                    EASY_BLOCK("glCompressedTexImage2D")
                    glCompressedTexImage2D(GL_TEXTURE_2D,
                           level,
                           TextureFormatToGLInternalFormat(m_Format),
                           mip.m_SizeX,
                           mip.m_SizeY,
                           0,
                           static_cast<int>(mip.m_Data.GetSize()),
                           mip.m_Data.GetData());
                    GL_CHECK_ERROR()
                }

                level++;
            }
        }
        else
        {
            if (SPARK_VERIFY(!IsCompressedFormat(m_Format)), "TextureResource::CreatePlatformResources - Compressed format without any mip data?")
            {
                EASY_BLOCK("glTexImage2D")
                glTexImage2D(GL_TEXTURE_2D,
                             0,
                             TextureFormatToGLInternalFormat(m_Format),
                             static_cast<GLsizei>(m_Width),
                             static_cast<GLsizei>(m_Height),
                             0,
                             TextureFormatToGLFormat(m_Format),
                             GL_UNSIGNED_BYTE,
                             nullptr);
                GL_CHECK_ERROR()
            }
        }

        EASY_BLOCK("Set Params")
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        GL_CHECK_ERROR()
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        GL_CHECK_ERROR()
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GL_CHECK_ERROR()
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GL_CHECK_ERROR()

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.f);
        GL_CHECK_ERROR()
        EASY_END_BLOCK

        if (!m_MipData.empty())
        {
            //EASY_BLOCK("glGenerateMipmap")
            //glGenerateMipmap(GL_TEXTURE_2D);
            //GL_CHECK_ERROR()
        }
    }

    GLuint TextureResource::TextureFormatToGLInternalFormat(asset::texture::Format format)
    {
        switch (format)
        {
            case asset::texture::Format::R8:
                return GL_R8;
            case asset::texture::Format::DXT5:
                return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            case asset::texture::Format::BC7:
                return GL_COMPRESSED_RGBA_BPTC_UNORM;
            case asset::texture::Format::BGRA8:
                return GL_BGRA;
            case asset::texture::Format::RGBA8:
                return GL_RGBA;
            case asset::texture::Format::Depth16:
                return GL_DEPTH_COMPONENT16;
            default:
                SPARK_ASSERT(false, "Unsupported GL format");
                return GL_INVALID_VALUE;
        }
    }

    void TextureResource::Bind(size_t)
    {
        glBindTexture(GL_TEXTURE_2D, m_ID);
    }

    GLuint TextureResource::TextureFormatToGLFormat(asset::texture::Format format)
    {
        switch (format)
        {
            case asset::texture::Format::R8:
                return GL_RED;
            case asset::texture::Format::DXT5:
                return GL_RGBA;
            case asset::texture::Format::BC7:
                return GL_RGBA;
            case asset::texture::Format::BGRA8:
                return GL_BGRA;
            case asset::texture::Format::RGBA8:
                return GL_RGBA;
            case asset::texture::Format::Depth16:
                return GL_DEPTH_COMPONENT;
            default:
                SPARK_ASSERT(false, "Unsupported GL format");
            return GL_INVALID_VALUE;

        }
    }
}
#endif
