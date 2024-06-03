#pragma once

#include "spark.h"
#include "engine/ecs/World.h"

#if SPARK_EDITOR
#include "EditorRuntime.h"
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
        void Run() const;
        virtual void Shutdown();

        IWindow* GetPrimaryWindow() { return m_PrimaryWindow; }
        ecs::World* GetWorld() { return &m_World; }

    protected:
        ecs::World m_World;

        virtual void Update();
        virtual void Render() {}

    private:

        PlatformRunLoop* m_RunLoop = nullptr;
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
