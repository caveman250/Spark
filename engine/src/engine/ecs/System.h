#pragma once

#include "World.h"
#include "engine/Application.h"
#include "engine/profiling/Profiler.h"

namespace se
{
    class Application;
}

namespace se::ecs
{
    class System : public reflect::ObjectBase
    {
    public:
        virtual ~System() {}

        virtual void OnInit(const SystemUpdateData&) {}
        virtual void OnUpdate(const SystemUpdateData&) {}
        virtual void OnRender(const SystemUpdateData&) {}
        virtual void OnShutdown(const SystemUpdateData&) {}

        template<typename Func>
        void RunQuery(Func&& func, bool force)
        {
            Application::Get()->GetWorld()->Each(m_Declaration.componentUsage, m_Declaration.singletonComponentUsage, func, force);
        }

        template<typename Func>
        void RunChildQuery(Id entity, const ChildQueryDeclaration& declaration, Func&& func)
        {
            Application::Get()->GetWorld()->ChildEach(entity, this, declaration, func);
        }

        template<typename Func>
        void RunRecursiveChildQuery(Id entity, const ChildQueryDeclaration& declaration, Func&& func)
        {
            Application::Get()->GetWorld()->RecursiveChildEach(entity, this, declaration, func);
        }

        bool DependsOn(Id other) const;
        const SystemDeclaration& GetDeclaration() const { return m_Declaration; }

    private:
        void Init();
        void Update();
        void Render();
        void Shutdown();

        SystemDeclaration m_Declaration;

        friend class World;
    };

    class EngineSystem : public System
    {
    };

    class AppSystem : public System
    {
    };
}
