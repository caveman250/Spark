#pragma once

#include "spark.h"
#include "engine/ecs/World.h"

#if SPARK_EDITOR
#include "editor/EditorRuntime.h"
#endif

namespace se
{
    class IWindow;
    class PlatformRunLoop;
    class Application
    {
    public:
        template <typename T>
        static void CreateInstance();
        static Application* Get();
        float GetDeltaTime() const { return m_DeltaTime; }

        virtual void Init();
        virtual void CreateInitialSingletonComponents();
        virtual void CreateInitialObservers();
        virtual void CreateInitialSystems();
        virtual void Shutdown();

        IWindow* GetPrimaryWindow() const { return m_PrimaryWindow; }
        ecs::World* GetWorld() { return &m_World; }

#if SPARK_EDITOR
        editor::EditorRuntime* GetEditorRuntime() { return &m_EditorRuntime; }
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
        editor::EditorRuntime m_EditorRuntime;
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
