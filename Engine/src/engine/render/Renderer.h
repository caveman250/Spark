#pragma once

#include "LightSetup.h"
#include "engine/memory/Arena.h"
#include "RenderCommand.h"
#include "RenderState.h"
#include "spark.h"

namespace se
{
    class IWindow;
}

namespace se::render
{
    template<typename T>
    concept ARenderCommand = std::is_base_of<commands::RenderCommand, T>::value;

    class Material;

    DECLARE_SPARK_ENUM_BEGIN(RenderAPI, int)
        OpenGL,
        Metal
    DECLARE_SPARK_ENUM_END()

    class Renderer
    {
    public:
        static Renderer* Create();
        static void Shutdown();
        template <typename T>
        static T* Get();
        virtual RenderAPI::Type GetRenderAPIType() const = 0;

        virtual ~Renderer() {}
        virtual void Init() = 0;

        template <ARenderCommand T, typename... Args>
        T* AllocRenderCommand(Args&&... args);

        template <ARenderCommand T, typename... Args>
        void Submit(IWindow* window, Args&&... args);
        void Submit(IWindow* window, commands::RenderCommand* renderCommand);
        const RenderState& GetCachedRenderState() const { return m_CachedRenderState; }
        const Material* GetBoundMaterial() const { return m_BoundMaterial; }
        void SetBoundMaterial(Material* material) { m_BoundMaterial = material; }
        void SetLastRenderState(const RenderState& rs);

        const LightSetup& GetLightSetup() const { return m_LightSetup; }
        void AddPointLight(const PointLight& light);

        void Update();

        virtual void Render(IWindow* window);
        virtual void EndFrame();
    protected:
        void SortDrawCommands(IWindow* window);
        void ExecuteDrawCommands(IWindow* window);

        std::unordered_map<IWindow*, std::vector<commands::RenderCommand*>> m_RenderCommands;

        memory::Arena m_RenderCommandsArena;

        RenderState m_CachedRenderState;
        Material* m_BoundMaterial;
        LightSetup m_LightSetup;

        static Renderer* s_Renderer;
    };

    template<ARenderCommand T, typename... Args>
    T* Renderer::AllocRenderCommand(Args &&... args)
    {
        return m_RenderCommandsArena.Alloc<T>(std::forward<Args>(args)...);
    }

    template<ARenderCommand T, typename... Args>
    void Renderer::Submit(IWindow* window, Args&&... args)
    {
        Submit(window, AllocRenderCommand<T>(std::forward<Args>(args)...));
    }

    template <typename T>
    T* Renderer::Get()
    {
        return static_cast<T*>(s_Renderer);
    }
}
