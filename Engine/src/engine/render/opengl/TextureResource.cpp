
#include <engine/render/TextureResource.h>

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
        glGenTextures(1, &m_ID);
        Bind();

        int level = 0;
        for (const auto& mip : m_Texture.GetMips())
        {
            glCompressedTexImage2D(GL_TEXTURE_2D,
                                   level,
                                   TextureFormatToGLFormat(m_Texture.GetFormat()),
                                   mip.m_SizeX,
                                   mip.m_SizeY,
                                   0,
                                   mip.m_Data.GetSize(),
                                   mip.m_Data.GetData());
            level++;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    GLuint TextureResource::TextureFormatToGLFormat(asset::texture::Format::Type format)
    {
        switch (format)
        {
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
}
