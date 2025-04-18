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
    class BaseSystem : public reflect::ObjectBase
    {
    public:
        virtual ~BaseSystem() {}

        virtual void Init() = 0;
        virtual void Update() = 0;
        virtual void Render() = 0;
        virtual void Shutdown() = 0;
        virtual std::map<Id, ComponentMutability::Type> GetUsedComponents() const = 0;

    protected:
        template<typename... Ts, typename Func>
        void RunQuery(Func&& func, bool force)
        {
            Application::Get()->GetWorld()->Each<Ts...>(func, m_Relationships, force);
        }

        template<typename... Ts, typename Func>
        void RunChildQuery(Id entity, Func&& func)
        {
            Application::Get()->GetWorld()->ChildEach<Ts...>(entity, this, func, m_Relationships);
        }

        template<typename... Ts>
        void RunVariantChildQuery(const Id& entity, const std::function<bool(std::variant<Ts*...>)>& func)
        {
            Application::Get()->GetWorld()->VariantChildEach<Ts...>(entity, this, func);
        }

        template<typename... Ts, typename Func>
        void RunRecursiveChildQuery(Id entity, Func&& func)
        {
            Application::Get()->GetWorld()->RecursiveChildEach<Ts...>(entity, this, func, m_Relationships);
        }

        const std::vector<Relationship>& GetRelationships() const { return m_Relationships; }
        const std::vector<ChildQuery>& GetChildQuery() const { return m_ChildQuery; }
        bool DependsOn(Id other) const;
        const std::unordered_set<Id>& GetDependencies() const { return m_DependsOn; }

    private:
        virtual void RegisterComponents() = 0;
        std::vector<Relationship> m_Relationships;
        std::vector<ChildQuery> m_ChildQuery;
        std::unordered_set<Id> m_DependsOn;

        friend class World;
    };

    inline bool BaseSystem::DependsOn(Id other) const
    {
        return m_DependsOn.contains(other);
    }

    template <typename... Cs>
    class System : public BaseSystem
    {
    public:
        System() {}

        void Init() override;
        void Update() override;
        void Render() override;
        void Shutdown() override;

        std::map<Id, ComponentMutability::Type> GetUsedComponents() const override;

    private:
        void RegisterComponents() override;

        virtual void OnInit(const std::vector<se::ecs::Id>&, Cs*...) {}
        virtual void OnUpdate(const std::vector<se::ecs::Id>&, Cs*...) {}
        virtual void OnRender(const std::vector<se::ecs::Id>&, Cs*...) {}
        virtual void OnShutdown(const std::vector<se::ecs::Id>&, Cs*...) {}

        template<std::size_t Index, typename... Ts>
        std::enable_if_t<Index != sizeof...(Cs) + 1> InitBuilder(Ts... ts);

        template<std::size_t Index, typename... Ts>
        std::enable_if_t<Index == sizeof...(Cs) + 1> InitBuilder(Ts... ts);

        template<std::size_t Index, typename... Ts>
        std::enable_if_t<Index != sizeof...(Cs) + 1> UpdateBuilder(Ts... ts);

        template<std::size_t Index, typename... Ts>
        std::enable_if_t<Index == sizeof...(Cs) + 1> UpdateBuilder(Ts... ts);

        template<std::size_t Index, typename... Ts>
        std::enable_if_t<Index != sizeof...(Cs) + 1> RenderBuilder(Ts... ts);

        template<std::size_t Index, typename... Ts>
        std::enable_if_t<Index == sizeof...(Cs) + 1> RenderBuilder(Ts... ts);

        template<std::size_t Index, typename... Ts>
        std::enable_if_t<Index != sizeof...(Cs) + 1> ShutdownBuilder(Ts... ts);

        template<std::size_t Index, typename... Ts>
        std::enable_if_t<Index == sizeof...(Cs) + 1> ShutdownBuilder(Ts... ts);
    };

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index == sizeof...(Cs) + 1> System<Cs...>::ShutdownBuilder(Ts... ts)
    {
        RunQuery<Cs...>(std::bind(&System::OnShutdown, this, ts...), true);
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index != sizeof...(Cs) + 1> System<Cs...>::ShutdownBuilder(Ts... ts)
    {
        ShutdownBuilder<Index + 1>(ts..., PLACEHOLDER(Index + 1)());
    }

    template<typename... Cs>
    void System<Cs...>::Shutdown()
    {
        PROFILE_SCOPE("System<Cs...>::Shutdown")
        ShutdownBuilder<1>(std::placeholders::_1);
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index == sizeof...(Cs) + 1> System<Cs...>::RenderBuilder(Ts... ts)
    {
       RunQuery<Cs...>(std::bind(&System::OnRender, this, ts...), false);
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index != sizeof...(Cs) + 1> System<Cs...>::RenderBuilder(Ts... ts)
    {
        RenderBuilder<Index + 1>(ts..., PLACEHOLDER(Index + 1)());
    }

    template<typename... Cs>
    void System<Cs...>::Render()
    {
        PROFILE_SCOPE("System<Cs...>::Render")
        RenderBuilder<1>(std::placeholders::_1);
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index == sizeof...(Cs) + 1> System<Cs...>::InitBuilder(Ts... ts)
    {
        RunQuery<Cs...>(std::bind(&System::OnInit, this, ts...), true);
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index != sizeof...(Cs) + 1> System<Cs...>::InitBuilder(Ts... ts)
    {
        InitBuilder<Index + 1>(ts..., PLACEHOLDER(Index + 1)());
    }

    template<typename... Cs>
    void System<Cs...>::Init()
    {
        PROFILE_SCOPE("System<Cs...>::Init")
        InitBuilder<1>(std::placeholders::_1);
    }

    template <typename T>
    void CollectUsedComponent(std::map<Id, ComponentMutability::Type>& vec)
    {
        Application::Get()->GetWorld()->RegisterComponent<T>();
        vec.insert(std::make_pair(T::GetComponentId(), std::is_const<T>() ? ComponentMutability::Immutable : ComponentMutability::Mutable));
    }

    template<typename... Cs>
    std::map<Id, ComponentMutability::Type> System<Cs...>::GetUsedComponents() const
    {
        std::map<Id, ComponentMutability::Type> ret;
        (CollectUsedComponent<Cs>(ret), ...);
        for (const auto& child : GetChildQuery())
        {
            ret.insert(std::make_pair(child.first, child.second));
        }
        return ret;
    }

    template <typename ... Cs>
    void System<Cs...>::RegisterComponents()
    {
        (Application::Get()->GetWorld()->RegisterComponent<Cs>(), ...);
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index != sizeof...(Cs) + 1> System<Cs...>::UpdateBuilder(Ts... ts)
    {
        UpdateBuilder<Index + 1>(ts..., PLACEHOLDER(Index + 1)());
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index == sizeof...(Cs) + 1> System<Cs...>::UpdateBuilder(Ts... ts)
    {
        RunQuery<Cs...>(std::bind(&System::OnUpdate, this, ts...), false);
    }

    template<typename... Cs>
    void System<Cs...>::Update()
    {
        PROFILE_SCOPE("System<Cs...>::Update")
        UpdateBuilder<1>(std::placeholders::_1);
    }

    template <typename... Cs>
    class EngineSystem : public System<Cs...>
    {
    private:
        template<typename... Ts, typename Func>
        void RunQuery(Func&&)
        {
            SPARK_ASSERT(false, "RunQuery should not be called irectly by systems");
        }
    };
    template <typename... Cs>
    class AppSystem : public System<Cs...>
    {
    private:
        template<typename... Ts, typename Func>
        void RunQuery(Func&&)
        {
            SPARK_ASSERT(false, "RunQuery should not be called irectly by systems");
        }
    };
}
