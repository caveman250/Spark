#pragma once

#include "spark.h"

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

        virtual void Init();
        void Run() const;
        virtual void Shutdown();

        IWindow* GetPrimaryWindow() { return m_PrimaryWindow; }

    private:

        virtual void Update();
        virtual void Render() {}

        PlatformRunLoop* m_RunLoop = nullptr;
        IWindow* m_PrimaryWindow = nullptr;

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
