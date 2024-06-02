#pragma once

#include "Component.h"
#include "World.h"

namespace se::ecs
{
    typedef uint64_t SystemId;

    class BaseSystem
    {
    public:
        virtual ~BaseSystem() {}

        virtual void Init() = 0;
        virtual void Update() = 0;
        virtual void Render() = 0;
        virtual void Shutdown() = 0;

    protected:
        World* m_World = nullptr;

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

        static std::vector<std::pair<ComponentId, bool>> GetUsedComponents();

    private:
        virtual void OnInit(const std::vector<se::ecs::EntityId>&, Cs*...) {}
        virtual void OnUpdate(const std::vector<se::ecs::EntityId>&, Cs*...) {}
        virtual void OnRender(const std::vector<se::ecs::EntityId>&, Cs*...) {}
        virtual void OnShutdown(const std::vector<se::ecs::EntityId>&, Cs*...) {}

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
        m_World->Each<Cs...>(std::bind(&System::OnShutdown, this, ts...), true);
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index != sizeof...(Cs) + 1> System<Cs...>::ShutdownBuilder(Ts... ts)
    {
        ShutdownBuilder<Index + 1>(ts..., std::_Ph<Index + 1>());
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
        m_World->Each<Cs...>(std::bind(&System::OnRender, this, ts...), false);
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index != sizeof...(Cs) + 1> System<Cs...>::RenderBuilder(Ts... ts)
    {
        RenderBuilder<Index + 1>(ts..., std::_Ph<Index + 1>());
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
        m_World->Each<Cs...>(std::bind(&System::OnInit, this, ts...), true);
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index != sizeof...(Cs) + 1> System<Cs...>::InitBuilder(Ts... ts)
    {
        InitBuilder<Index + 1>(ts..., std::_Ph<Index + 1>());
    }

    template<typename... Cs>
    void System<Cs...>::Init()
    {
        InitBuilder<1>(std::placeholders::_1);
    }

    template <typename T>
    void CollectUsedComponent(std::vector<std::pair<ComponentId, bool>>& vec)
    {
        vec.push_back(std::make_pair(T::GetComponentId(), std::is_const<T>()));
    }

    template<typename... Cs>
    std::vector<std::pair<ComponentId, bool>> System<Cs...>::GetUsedComponents()
    {
        std::vector<std::pair<ComponentId, bool>> ret;
        (CollectUsedComponent<Cs>(ret), ...);
        return ret;
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index != sizeof...(Cs) + 1> System<Cs...>::UpdateBuilder(Ts... ts)
    {
        UpdateBuilder<Index + 1>(ts..., std::_Ph<Index + 1>());
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index == sizeof...(Cs) + 1> System<Cs...>::UpdateBuilder(Ts... ts)
    {
        m_World->Each<Cs...>(std::bind(&System::OnUpdate, this, ts...), false);
    }

    template<typename... Cs>
    void System<Cs...>::Update()
    {
        UpdateBuilder<1>(std::placeholders::_1);
    }
}
