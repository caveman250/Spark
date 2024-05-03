#pragma once


namespace se::editor::startup
{
    class StartupTask
    {
    public:
        virtual ~StartupTask() {}
        virtual void Run() = 0;
    };
}
