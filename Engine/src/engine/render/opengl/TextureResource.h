#pragma once

#include "engine/render/render_fwd.h"

#if OPENGL_RENDERER

#include "engine/render/TextureResource.h"
#include "GL_fwd.h"

namespace se::render::opengl
{
    class TextureResource : public render::TextureResource
    {
    public:
        TextureResource(const asset::Texture& texture) : render::TextureResource(texture) {}
        void CreatePlatformResources() override;
        void Bind(size_t i) override;

        GLuint GetGLID() const { return m_ID; }

    private:

        static GLuint TextureFormatToGLFormat(asset::texture::Format::Type format);
        static GLuint TextureFormatToGLInternalFormat(asset::texture::Format::Type format);

        GLuint m_ID;
    };
}

#endif
