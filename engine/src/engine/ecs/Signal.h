#pragma once
#include "engine/Application.h"

namespace se::ecs
{
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

        SignalHandle Subscribe(std::function<void(Cs...)>&& func);

        void Unsubscribe(SignalHandle handle);

        void Broadcast(Cs... cs);

    private:
        void Execute() override;

        template <size_t... Is>
        void CallFunc(const std::function<void(Cs...)>& func, std::index_sequence<Is...>);

        SignalHandle m_HandleCounter;
        std::vector<std::pair<SignalHandle, std::function<void(Cs...)>>> m_RegisteredHandles;
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
        auto it = std::ranges::find(m_RegisteredHandles, handle);
        if (it != m_RegisteredHandles.end())
        {
            m_RegisteredHandles.erase(it);
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
            CallFunc(func, std::make_index_sequence<sizeof...(Cs)>());
        }

        m_PendingInvokeIndex++;

        if (m_PendingInvokeIndex >= m_PendingInvokeArgs.size())
        {
            m_PendingInvokeArgs.clear();
            m_PendingInvokeIndex = 0;
        }
    }

    template<typename ... Cs>
    template<size_t... Is>
    void Signal<Cs...>::CallFunc(const std::function<void(Cs...)>& func, std::index_sequence<Is...>)
    {
        func(std::get<Is>(m_PendingInvokeArgs[m_PendingInvokeIndex])...);
    }

    template <typename... Cs>
    SignalHandle Signal<Cs...>::Subscribe(std::function<void(Cs...)> &&func)
    {
        SignalHandle id = m_HandleCounter++;

        m_RegisteredHandles.push_back(std::make_pair(id, std::move(func)));
        return id;
    }
}
