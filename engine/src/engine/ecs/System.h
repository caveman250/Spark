#pragma once

#include "World.h"
#include "engine/Application.h"
#include "SystemUtil.h"

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
        virtual UpdateMode GetUpdateMode() const { return UpdateMode::MultiThreaded; }

        template<typename Func>
        void RunQuery(Func&& func, bool force)
        {
            Application::Get()->GetWorld()->Each(m_Declaration.componentUsage, m_Declaration.variantComponentUsage, m_Declaration.singletonComponentUsage, func, GetUpdateMode(), force);
        }

        template<typename Func>
        void RunChildQuery(const Id& entity, const HeirachyQueryDeclaration& declaration, Func&& func)
        {
            Application::Get()->GetWorld()->ChildEach(entity, this, declaration, func);
        }

        template<typename Func>
        void RunQueryOnChild(const Id& child, const HeirachyQueryDeclaration& declaration, Func&& func)
        {
            Application::Get()->GetWorld()->HeirachyQuery(child, this, declaration, func);
        }

        template<typename Func>
        void RunQueryOnParent(const Id& entity, const HeirachyQueryDeclaration& declaration, Func&& func)
        {
            Application::Get()->GetWorld()->ParentQuery(entity, this, declaration, func);
        }

        template<typename Func>
        void RunRecursiveChildQuery(const Id& entity, const HeirachyQueryDeclaration& declaration, Func&& func)
        {
            Application::Get()->GetWorld()->RecursiveChildEach(entity, this, declaration, func);
        }

        bool DependsOn(const Id& other) const;
        const SystemDeclaration& GetDeclaration() const { return m_Declaration; }
        virtual bool ImplementsUpdateMethod() const { return false; }
        virtual bool ImplementsRenderMethod() const { return false; }

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
