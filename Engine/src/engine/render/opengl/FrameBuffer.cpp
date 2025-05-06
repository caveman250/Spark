#include "FrameBuffer.h"
#include "engine/asset/texture/Texture.h"
#include "engine/render/opengl/TextureResource.h"

namespace se::render
{
    std::shared_ptr<FrameBuffer> FrameBuffer::CreateFrameBuffer()
    {
        return std::make_shared<opengl::FrameBuffer>();
    }
}

namespace se::render::opengl
{
    FrameBuffer::FrameBuffer()
        : render::FrameBuffer()
    {
        glGenFramebuffers(1, &m_PlatformResource);
        glBindFramebuffer(GL_FRAMEBUFFER, m_PlatformResource);

        const auto& colorTextureResource = static_pointer_cast<TextureResource>(m_ColorTexture->GetPlatformResource());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextureResource->GetGLID(), 0);
        GL_CHECK_ERROR()

        const auto& depthTextureResource = static_pointer_cast<TextureResource>(m_DepthTexture->GetPlatformResource());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureResource->GetGLID(), 0);
        GL_CHECK_ERROR()

        [[maybe_unused]] auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        SPARK_ASSERT(status == GL_FRAMEBUFFER_COMPLETE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_PlatformResource);
        GL_CHECK_ERROR()
    }

    void FrameBuffer::Commit()
    {
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            debug::Log::Error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GL_CHECK_ERROR()
    }
}