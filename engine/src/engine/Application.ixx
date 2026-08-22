module;

#include "spark.h"
#include "engine/ecs/World.h"
#include "platform/IWindow.h"

export module Application;
import Spark.Math;

#if SPARK_EDITOR
import Editor;
#endif

namespace se
{
    export class Application
    {
    public:
        template <typename T>
        static void CreateInstance();
        static Application* Get();
        float GetDeltaTime() const { return m_DeltaTime; }

        virtual void Init();
        virtual void CreateInitialSingletonComponents();
        virtual void CreateInitialObservers();
        virtual void Shutdown();

        IWindow* GetWindow() const { return m_PrimaryWindow; }
        math::IntVec2 GetGameViewportSize();
        ecs::World* GetWorld() { return &m_World; }

#if SPARK_EDITOR
        editor::Editor* GetEditor() { return &m_Editor; }
#endif

    protected:
        ecs::World m_World;

        virtual void Update();
        virtual void Render();

    private:
        IWindow* m_PrimaryWindow = nullptr;

        std::chrono::time_point<std::chrono::system_clock> m_TimeLastFrame = {};
        float m_DeltaTime = {};

        static Application* s_Instance;

        friend class PlatformRunLoop;

#if SPARK_EDITOR
        editor::Editor m_Editor;
#endif
    };

    template <typename T>
    void Application::CreateInstance()
    {
        if (SPARK_VERIFY(!s_Instance, "Application::CreateInstance - Instance already exists"))
        {
            s_Instance = new T();
        }
    }
}
