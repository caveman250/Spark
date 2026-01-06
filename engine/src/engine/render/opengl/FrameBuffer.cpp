#include "FrameBuffer.h"

#include "engine/Application.h"
#include "engine/asset/texture/Texture.h"
#include "engine/render/opengl/TextureResource.h"
#include "platform/IWindow.h"

#if OPENGL_RENDERER

namespace se::render
{
    std::shared_ptr<FrameBuffer> FrameBuffer::CreateFrameBuffer(const math::IntVec2& size)
    {
        return std::make_shared<opengl::FrameBuffer>(size);
    }
}

namespace se::render::opengl
{
    FrameBuffer::FrameBuffer(const math::IntVec2& size)
        : render::FrameBuffer(size)
    {
        glGenFramebuffers(1, &m_PlatformResource);
        glBindFramebuffer(GL_FRAMEBUFFER, m_PlatformResource);

        const auto& colorTextureResource = std::static_pointer_cast<TextureResource>(m_ColorTexture->GetPlatformResource());
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextureResource->GetGLID(), 0);
        GL_CHECK_ERROR()

        const auto& depthTextureResource = std::static_pointer_cast<TextureResource>(m_DepthTexture->GetPlatformResource());
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

        glViewport(0, 0, m_Size.x, m_Size.y);
    }

    void FrameBuffer::UnBind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        GL_CHECK_ERROR()

        auto window = Application::Get()->GetWindow();
        glViewport(0, 0, window->GetWidth(), window->GetHeight());
    }

    void FrameBuffer::Commit()
    {
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            debug::Log::Error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }
}

#endif