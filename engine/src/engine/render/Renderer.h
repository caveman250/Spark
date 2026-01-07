#pragma once

#include "LightSetup.h"
#include "engine/memory/Arena.h"
#include "RenderCommand.h"
#include "RenderState.h"
#include "spark.h"
#include "RenderGroup.h"
#include "engine/threads/SpinLock.h"

namespace se::render
{
    template<typename T>
    concept ARenderCommand = std::is_base_of<commands::RenderCommand, T>::value;

    class FrameBuffer;
    class Material;

    SPARK_ENUM()
    enum class RenderAPI
    {
        OpenGL,
        Metal
    };

    class Renderer
    {
    public:
        static Renderer* Create();
        static void Shutdown();
        template <typename T>
        static T* Get();
        virtual RenderAPI GetRenderAPIType() const = 0;

        virtual ~Renderer() {}
        virtual void Init() = 0;

        template <ARenderCommand T, typename... Args>
        T* AllocRenderCommand(Args&&... args);

        template <ARenderCommand T, typename... Args>
        void Submit(size_t group, Args&&... args);

        template <ARenderCommand T, typename... Args>
        void Submit(Args&&... args);

        void Submit(size_t group, commands::RenderCommand* renderCommand);
        const RenderState& GetCachedRenderState() const { return m_CachedRenderState; }
        const Material* GetBoundMaterial() const { return m_RenderGroups[m_ActiveRenderGroup].boundMaterial; }
        void SetBoundMaterial(Material* material) { m_RenderGroups[m_ActiveRenderGroup].boundMaterial = material; }
        void SetLastRenderState(const RenderState& rs);

        const LightSetup& GetLightSetup() const { return m_LightSetup; }
        void AddPointLight(const PointLight& light);

        size_t AllocRenderGroup(int layer);
        size_t GetDefaultRenderGroup() { return m_DefaultRenderGroup; }

        virtual void SetFrameBuffer(size_t group, const std::shared_ptr<FrameBuffer>& fb);

        void Update();

        virtual void Render();
        virtual void EndFrame();
    protected:

        void SortDrawCommands();
        void ExecuteDrawCommands();

        std::vector<RenderGroup> m_RenderGroups = {};

        memory::Arena m_RenderCommandsArena = memory::Arena(20000000);

        RenderState m_CachedRenderState = {};
        LightSetup m_LightSetup = {};
        size_t m_DefaultRenderGroup = 0;
        size_t m_ActiveRenderGroup = 0;

        threads::SpinLock m_RenderGroupMutex = {};
        threads::SpinLock m_RenderCommandMutex = {};

        static Renderer* s_Renderer;
    };

    template<ARenderCommand T, typename... Args>
    T* Renderer::AllocRenderCommand(Args &&... args)
    {
        m_RenderCommandMutex.Lock();
        T* ret = m_RenderCommandsArena.Alloc<T>(std::forward<Args>(args)...);
        m_RenderCommandMutex.Unlock();
        return ret;
    }

    template<ARenderCommand T, typename... Args>
    void Renderer::Submit(size_t group, Args&&... args)
    {
        Submit(group, AllocRenderCommand<T>(std::forward<Args>(args)...));
    }

    template<ARenderCommand T, typename... Args>
    void Renderer::Submit(Args &&... args)
    {
        Submit(GetDefaultRenderGroup(), AllocRenderCommand<T>(std::forward<Args>(args)...));
    }

    template <typename T>
    T* Renderer::Get()
    {
        return static_cast<T*>(s_Renderer);
    }
}
