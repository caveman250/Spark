#pragma once

#include "engine/render/TextureResource.h"
#include "GL_fwd.h"

namespace se::render::opengl
{
    class TextureResource : public render::TextureResource
    {
    public:
        TextureResource(const asset::Texture& texture) : render::TextureResource(texture) {}
        void CreatePlatformResources() override;
        void Bind() override;

    private:

        static GLuint TextureFormatToGLFormat(asset::texture::Format::Type format);

        GLuint m_ID;
    };
}
