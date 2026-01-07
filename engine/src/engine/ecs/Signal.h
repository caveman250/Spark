#pragma once
#include "System.h"
#include "engine/Application.h"

namespace se::ecs
{
    typedef uint64_t RuntimeSignalHandle;

    struct SignalHandle
    {
        SPARK_POD_CLASS()

        SPARK_MEMBER(Serialized)
        std::string systemName = {};

        SPARK_MEMBER(Serialized)
        std::string functionName = {};

        auto operator==(const SignalHandle& rhs) const
        {
            return systemName == rhs.systemName &&
                functionName == rhs.functionName;
        }
    };

    class BaseSignal : public reflect::ObjectBase
    {
    private:
        virtual void Execute() = 0;

        friend class World;
    };

    template <typename... Cs>
    class Signal : public BaseSignal
    {
        SPARK_CLASS_TEMPLATED()
    public:
        ~Signal();

        // Runtime only
        RuntimeSignalHandle Subscribe(std::function<void(Cs...)>&& func);
        void Unsubscribe(RuntimeSignalHandle handle);

        // Serialised (System methods only!)
        const SignalHandle& Subscribe(const std::string& systemName, const std::string& functionName);
        void Unsubscribe(const SignalHandle& handle);

        void Broadcast(Cs... cs);

    private:
        template <size_t I>
        void CollectArg(std::vector<std::any>& arg_vec);
        template <size_t... Is>
        void CollectArgs(std::vector<std::any>& arg_vec, std::index_sequence<Is...>);

        void Execute() override;

        template <size_t... Is>
        void CallFunc(const std::function<void(Cs...)>& func, std::index_sequence<Is...>);

        RuntimeSignalHandle m_HandleCounter;
        std::vector<std::pair<RuntimeSignalHandle, std::function<void(Cs...)>>> m_RegisteredRuntimeHandles;

        SPARK_MEMBER(Serialized)
        std::vector<SignalHandle> m_RegisteredHandles = {};

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
    void Signal<Cs...>::Unsubscribe(RuntimeSignalHandle handle)
    {
        auto it = std::ranges::find(m_RegisteredRuntimeHandles, handle);
        if (it != m_RegisteredRuntimeHandles.end())
        {
            m_RegisteredRuntimeHandles.erase(it);
        }
    }

    template <typename... Cs>
    void Signal<Cs...>::Broadcast(Cs... cs)
    {
        m_PendingInvokeArgs.push_back(std::make_tuple(cs...));
        Application::Get()->GetWorld()->AddPendingSignal(this);
    }

    template<typename ... Cs>
    template<size_t I>
    void Signal<Cs...>::CollectArg(std::vector<std::any>& arg_vec)
    {
        arg_vec.push_back(std::get<I>(m_PendingInvokeArgs[m_PendingInvokeIndex]));
    }

    template<typename ... Cs>
    template<size_t... Is>
    void Signal<Cs...>::CollectArgs(std::vector<std::any>& arg_vec, std::index_sequence<Is...>)
    {
        (CollectArg<Is>(arg_vec), ...);
    }

    template <typename... Cs>
    void Signal<Cs...>::Execute()
    {
        for (const auto& [id, func] : m_RegisteredRuntimeHandles)
        {
            CallFunc(func, std::make_index_sequence<sizeof...(Cs)>());
        }

        if (!m_RegisteredHandles.empty())
        {
            auto world = Application::Get()->GetWorld();
            std::vector<std::any> arg_vec = {};
            arg_vec.reserve(sizeof...(Cs));
            CollectArgs(arg_vec, std::make_index_sequence<sizeof...(Cs)>());

            for (const auto& signal : m_RegisteredHandles)
            {
                const auto reflect = static_cast<reflect::System*>(reflect::TypeFromString(signal.systemName));
                const auto system = world->GetAppSystem(reflect->GetStaticId());
                system->Invoke(signal.functionName, arg_vec);
            }
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
    RuntimeSignalHandle Signal<Cs...>::Subscribe(std::function<void(Cs...)> &&func)
    {
        RuntimeSignalHandle id = m_HandleCounter++;

        m_RegisteredRuntimeHandles.push_back(std::make_pair(id, std::move(func)));
        return id;
    }

    template<typename ... Cs>
    const SignalHandle& Signal<Cs...>::Subscribe(const std::string& systemName,
        const std::string& functionName)
    {
        return m_RegisteredHandles.emplace_back(SignalHandle
            {
                .systemName = systemName,
                .functionName = functionName
            });
    }

    template<typename ... Cs>
    void Signal<Cs...>::Unsubscribe(const SignalHandle& handle)
    {
        auto [first, last] = std::ranges::remove(m_RegisteredHandles, handle);
        m_RegisteredHandles.erase(first, last);
    }
}

#include "se_ecs_Signal.generated.h"