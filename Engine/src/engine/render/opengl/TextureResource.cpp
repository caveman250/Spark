
#include "engine/render/TextureResource.h"

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
        render::TextureResource::CreatePlatformResources();
        glGenTextures(1, &m_ID);
        GL_CHECK_ERROR()
        glBindTexture(GL_TEXTURE_2D, m_ID);
        GL_CHECK_ERROR()

        int level = 0;
        for (const auto& mip : m_Texture.GetMips())
        {
            if (!IsCompressedFormat(m_Texture.GetFormat()))
            {
                glTexImage2D(GL_TEXTURE_2D,
                                       level,
                                       TextureFormatToGLInternalFormat(m_Texture.GetFormat()),
                                       mip.m_SizeX,
                                       mip.m_SizeY,
                                       0,
                                       TextureFormatToGLFormat(m_Texture.GetFormat()),
                                       GL_UNSIGNED_BYTE,
                                       mip.m_Data.GetData());
                GL_CHECK_ERROR()
            }
            else
            {
                glCompressedTexImage2D(GL_TEXTURE_2D,
                       level,
                       TextureFormatToGLInternalFormat(m_Texture.GetFormat()),
                       mip.m_SizeX,
                       mip.m_SizeY,
                       0,
                       static_cast<int>(mip.m_Data.GetSize()),
                       mip.m_Data.GetData());
                GL_CHECK_ERROR()
            }

            level++;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        GL_CHECK_ERROR()
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        GL_CHECK_ERROR()
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GL_CHECK_ERROR()
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        GL_CHECK_ERROR()
        glGenerateMipmap(GL_TEXTURE_2D);
        GL_CHECK_ERROR()
    }

    GLuint TextureResource::TextureFormatToGLInternalFormat(asset::texture::Format::Type format)
    {
        switch (format)
        {
            case asset::texture::Format::R8:
                return GL_R8;
            case asset::texture::Format::DXT5:
                return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            default:
                SPARK_ASSERT(false, "Unsupported GL format");
                return GL_INVALID_VALUE;
        }
    }

    void TextureResource::Bind()
    {
        glBindTexture(GL_TEXTURE_2D, m_ID);
    }

    bool TextureResource::IsCompressedFormat(asset::texture::Format::Type format)
    {
        switch (format)
        {
            case asset::texture::Format::R8:
                return false;
            case asset::texture::Format::DXT5:
                return true;
            default:
                SPARK_ASSERT(false, "Unsupported GL format");
                return false;
        }
    }

    GLuint TextureResource::TextureFormatToGLFormat(asset::texture::Format::Type format)
    {
        switch (format)
        {
            case asset::texture::Format::R8:
                return GL_RED;
            case asset::texture::Format::DXT5:
                return GL_RGBA;
            default:
                SPARK_ASSERT(false, "Unsupported GL format");
            return GL_INVALID_VALUE;
        }
    }
}
