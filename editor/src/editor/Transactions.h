#pragma once
#include "spark.h"

namespace se::editor
{
    template <typename T>
    concept Invokable = std::is_same_v<std::invoke_result_t<T>, void>;

    template <typename F, typename T>
    concept CallableWithReturnType = std::invocable<F> &&
                                  std::same_as<std::invoke_result_t<F>, int>;


    struct Transaction
    {
        std::function<void()> apply;
        std::function<void()> undo;
    };

    class Transactions
    {
    public:
        static Transactions* Get();

        template <Invokable Apply, Invokable Undo>
        void PushAction(Apply&& apply, Undo&& undo);

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
        apply();
        m_Transactions.emplace_back(Transaction
            {
                .apply = apply,
                .undo = undo
            });
        m_TransactionIndex++;
    }
}
