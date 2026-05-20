#pragma once
#include "spark.h"

namespace se::editor
{
    template <typename T>
    concept Invokable = std::is_same_v<std::invoke_result_t<T>, void>;

    struct Transaction
    {
        std::function<void()> apply = nullptr;
        std::function<void()> undo = nullptr;
        void* state = nullptr;
    };

    class Transactions
    {
    public:
        static Transactions* Get();

        template <Invokable Apply, Invokable Undo>
        void PushAction(Apply&& apply, Undo&& undo);

        template<typename T, Invokable Apply, Invokable Undo, typename... StateArgs>
        void PushAction(Apply&& apply, Undo&& undo, StateArgs&&... args);

        template <typename T>
        T* GetUndoState();

        template <typename T>
        T* GetRedoState();

        void Undo();
        void Redo();

    private:
        void InvalidateFrom(size_t i);

        std::mutex m_Mutex = {};
        std::vector<Transaction> m_Transactions = {};
        size_t m_TransactionIndex = 0;
    };

    template<Invokable Apply, Invokable Revert>
    void Transactions::PushAction(Apply&& apply,
        Revert&& undo)
    {
        auto lock = std::lock_guard(m_Mutex);
        InvalidateFrom(m_TransactionIndex);
        m_Transactions.emplace_back(Transaction
            {
                .apply = apply,
                .undo = undo
            });
        apply();
        m_TransactionIndex++;
    }

    template<typename T, Invokable Apply, Invokable Revert, typename... StateArgs>
    void Transactions::PushAction(Apply&& apply,
        Revert&& undo,
        StateArgs&&... args)
    {
        auto lock = std::lock_guard(m_Mutex);
        InvalidateFrom(m_TransactionIndex);
        m_Transactions.emplace_back(Transaction
            {
                .apply = apply,
                .undo = undo,
                .state = new T(std::forward<StateArgs>(args)...)
            });
        apply();
        m_TransactionIndex++;

    }

    template<typename T>
    T* Transactions::GetUndoState()
    {
        return static_cast<T*>(m_Transactions[m_TransactionIndex - 1].state);
    }

    template<typename T>
    T* Transactions::GetRedoState()
    {
        return static_cast<T*>(m_Transactions[m_TransactionIndex].state);
    }
}
