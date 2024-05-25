#pragma once

#include "Component.h"
#include "World.h"

namespace se::ecs
{
    typedef uint64_t SystemId;

    class BaseSystem
    {
    public:
        virtual void Update(float dt) = 0;
        virtual std::vector<std::pair<ComponentId, bool>> GetUsedComponents() = 0;
    };

    template <typename... Cs>
    class System : public BaseSystem
    {
    public:
        System(World& world);

        void Update(float dt) override;

        std::vector<std::pair<ComponentId, bool>> GetUsedComponents() override;

    private:
        virtual void OnUpdate(float dt, size_t count, Cs*... ts) = 0;

        template<std::size_t Index, typename... Ts>
        std::enable_if_t<Index != sizeof...(Cs) + 1> UpdateBuilder(float dt, Ts... ts);

        template<std::size_t Index, typename... Ts>
        std::enable_if_t<Index == sizeof...(Cs) + 1> UpdateBuilder(float dt, Ts... ts);

        World& m_World;
    };

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
    System<Cs...>::System(World& world)
        : m_World(world)
    {

    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index != sizeof...(Cs) + 1> System<Cs...>::UpdateBuilder(float dt, Ts... ts)
    {
        using IthT = std::tuple_element<Index - 1, std::tuple<Cs...>>::type;

        UpdateBuilder<Index + 1>(dt,
                            ts...,
                            std::_Ph<Index + 2>());
    }

    template<typename... Cs>
    template<std::size_t Index, typename... Ts>
    std::enable_if_t<Index == sizeof...(Cs) + 1> System<Cs...>::UpdateBuilder(float dt, Ts... ts)
    {
        m_World.Each<Cs...>(dt, std::bind(&System::OnUpdate, this, ts...));
    }

    template<typename... Cs>
    void System<Cs...>::Update(float dt)
    {
        UpdateBuilder<1>(dt, std::placeholders::_1, std::placeholders::_2);
    }
}
