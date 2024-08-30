#pragma once
#include "SignalActionBuilder.h"
#include "engine/Application.h"

namespace se::ecs
{
    typedef uint64_t Id;
    typedef uint64_t SignalHandle;

    class BaseSignal
    {
    private:
        virtual void Execute() = 0;

        friend class World;
    };

    template <typename... Cs>
    class Signal : public BaseSignal
    {
    public:
        ~Signal();

        template<typename... Ts>
        SignalHandle Subscribe(Id entityId, std::function<void(Cs..., Ts*...)>&& func);

        void Unsubscribe(SignalHandle handle);

        void Broadcast(Cs... cs);

    private:
        void Execute() override;

        template <size_t... Is>
        void CallWrapperFunc(const std::function<void(Cs...)>& func, std::index_sequence<Is...>);

        SignalHandle m_HandleCounter;
        std::unordered_map<SignalHandle, std::function<void(Cs...)>> m_RegisteredHandles;
        std::vector<std::tuple<Cs...>> m_PendingInvokeArgs;
        size_t m_PendingInvokeIndex = 0;
    };

    template <typename T>
    void CollectComponentId(std::vector<Id>& vec)
    {
        vec.push_back(T::GetComponentId());
    }

    template <typename... Cs>
    Signal<Cs...>::~Signal()
    {
        Application::Get()->GetWorld()->OnSignalDestroyed(this);
    }

    template <typename... Cs>
    void Signal<Cs...>::Unsubscribe(SignalHandle handle)
    {
        if (m_RegisteredHandles.contains(handle))
        {
            m_RegisteredHandles.erase(handle);
        }
    }

    template <typename... Cs>
    void Signal<Cs...>::Broadcast(Cs... cs)
    {
        m_PendingInvokeArgs.push_back(std::make_tuple(cs...));
        Application::Get()->GetWorld()->AddPendingSignal(this);
    }

    template <typename... Cs>
    void Signal<Cs...>::Execute()
    {
        for (const auto& [id, func] : m_RegisteredHandles)
        {
            CallWrapperFunc(func, std::make_index_sequence<sizeof...(Cs)>());
        }

        m_PendingInvokeIndex++;

        if (m_PendingInvokeIndex >= m_PendingInvokeArgs.size() - 1)
        {
            m_PendingInvokeArgs.clear();
            m_PendingInvokeIndex = 0;
        }
    }

    template<typename ... Cs>
    template<size_t... Is>
    void Signal<Cs...>::CallWrapperFunc(const std::function<void(Cs...)>& func, std::index_sequence<Is...>)
    {
        func(std::get<Is>(m_PendingInvokeArgs[m_PendingInvokeIndex])...);
    }

    template <typename... Cs>
    template <typename ... Ts>
    SignalHandle Signal<Cs...>::Subscribe(Id entityId, std::function<void(Cs..., Ts*...)> &&func)
    {
        SignalHandle id = m_HandleCounter++;

        auto wrapperFunc = [this, entityId, wrappedFunc = std::move(func)](Cs... cs)
        {
            auto* world = Application::Get()->GetWorld();

            std::vector<Id> comps;
            comps.reserve(sizeof...(Ts));
            (CollectComponentId<Ts>(comps), ...);

            for (const auto& comp : comps)
            {
                if (!world->HasComponent(entityId, comp))
                {
                    return;
                }
            }

            SignalActionBuilder<Ts...> builder;
            builder.DoAction(entityId, world, wrappedFunc, cs...);
        };

        m_RegisteredHandles.insert(std::make_pair(id, std::move(wrapperFunc)));
        return id;
    }
}
