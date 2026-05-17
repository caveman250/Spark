#include "Transactions.h"

namespace se::editor
{
    Transactions* Transactions::Get()
    {
        static Transactions* s_Instance = nullptr;
        if (!s_Instance)
        {
            s_Instance = new Transactions();
        }

        return s_Instance;
    }

    void Transactions::Undo()
    {
        if (m_TransactionIndex > 0)
        {
            const auto& transaction = m_Transactions[m_TransactionIndex - 1];
            transaction.undo();
            m_TransactionIndex--;
        }
    }

    void Transactions::Redo()
    {
        if (m_TransactionIndex < m_Transactions.size())
        {
            const auto& transaction = m_Transactions[m_TransactionIndex];
            transaction.apply();
            m_TransactionIndex++;
        }
    }

    void Transactions::InvalidateFrom(const size_t i)
    {
        if (i < m_Transactions.size())
        {
            m_Transactions.erase(m_Transactions.begin() + i, m_Transactions.end());
        }
    }
}
