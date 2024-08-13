#pragma once

#include "World.h"
#include "engine/Application.h"

namespace se
{
    class Application;
}

namespace se::ecs
{
    typedef uint64_t Id;

    class BaseSystem : public reflect::ObjectBase
    {
    public:
        virtual ~BaseSystem() {}

        virtual void Init() = 0;
        virtual void Update() = 0;
        virtual void Render() = 0;
        virtual void Shutdown() = 0;
        virtual std::map<Id, bool> GetUsedComponents() const = 0;

    protected:
        World* m_World = nullptr;
        std::vector<Relationship> m_Relationships;

        template<typename... Ts, typename Func>
        void RunQuery(Func&& func)
        {
            m_World->Each<Ts...>(func, m_Relationships, true);
        }

        friend class World;
    };

    template <typename... Cs>
    class System : public BaseSystem
    {
    public:
        System() {}

        void Init() override;
        void Update() override;
        void Render() override;
        void Shutdown() override;

        std::map<Id, bool> GetUsedComponents() const override;

    private:
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
        RunQuery<Cs...>(std::bind(&System::OnShutdown, this, ts...));
    }

#if SPARK_PLATFORM_WINDOWS
#define PLACEHOLDER(i) std::_Ph<i>
#else
#define PLACEHOLDER(i) std::_Placeholder<i>
#endif

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index != sizeof...(Cs) + 1> System<Cs...>::ShutdownBuilder(Ts... ts)
    {
        ShutdownBuilder<Index + 1>(ts..., PLACEHOLDER(Index + 1)());
    }

    template<typename... Cs>
    void System<Cs...>::Shutdown()
    {
        ShutdownBuilder<1>(std::placeholders::_1);
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index == sizeof...(Cs) + 1> System<Cs...>::RenderBuilder(Ts... ts)
    {
       RunQuery<Cs...>(std::bind(&System::OnRender, this, ts...));
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
        RenderBuilder<1>(std::placeholders::_1);
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index == sizeof...(Cs) + 1> System<Cs...>::InitBuilder(Ts... ts)
    {
        RunQuery<Cs...>(std::bind(&System::OnInit, this, ts...));
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
        InitBuilder<1>(std::placeholders::_1);
    }

    template <typename T>
    void CollectUsedComponent(std::map<Id, bool>& vec)
    {
        Application::Get()->GetWorld()->RegisterComponent<T>();
        vec.insert(std::make_pair(T::GetComponentId(), std::is_const<T>()));
    }

    template<typename... Cs>
    std::map<Id, bool> System<Cs...>::GetUsedComponents() const
    {
        std::map<Id, bool> ret;
        (CollectUsedComponent<Cs>(ret), ...);
        return ret;
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
        RunQuery<Cs...>(std::bind(&System::OnUpdate, this, ts...));
    }

    template<typename... Cs>
    void System<Cs...>::Update()
    {
        UpdateBuilder<1>(std::placeholders::_1);
    }

    template <typename... Cs>
    class EngineSystem : public System<Cs...> {};
    template <typename... Cs>
    class AppSystem : public System<Cs...> {};
}
