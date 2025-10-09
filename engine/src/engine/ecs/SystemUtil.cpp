#include "SystemUtil.h"

#include "System.h"

namespace se::ecs
{
    int SystemSorter::AddVertex(const int vertex)
    {
        m_Vertices[m_NumVerts++] = vertex;
        return m_NumVerts - 1;
    }

    void SystemSorter::AddEdge(const int start,
        const int end)
    {
        m_Matrix[start][end] = 1;
    }

    std::vector<std::pair<ecs::Id, SystemRecord>> SystemSorter::Sort(const std::unordered_map<ecs::Id, SystemRecord>& systems)
    {
        m_Vertices.resize(systems.size());
        m_Matrix.resize(systems.size());
        for (size_t i = 0; i < systems.size(); i++)
        {
            m_Matrix[i].resize(systems.size());
        }
        m_SortedArray.reserve(systems.size());
        m_NumVerts = 0;

        std::map<ecs::Id, int> indices = {};

        std::vector<std::pair<ecs::Id, SystemRecord>> systemList = {};
        std::ranges::for_each(systems, [&systemList](const std::pair<ecs::Id, SystemRecord>& kvp)
        {
            systemList.push_back(kvp);
        });

        for (size_t i = 0; i < systemList.size(); ++i)
        {
            indices[systemList[i].first] = AddVertex(i);
        }

        for (size_t i = 0; i < systemList.size(); ++i)
        {
            for (size_t j = 0; j < systemList[i].second.instance->GetDeclaration().dependencies.size(); j++)
            {
                AddEdge(i, indices[systemList[i].second.instance->GetDeclaration().dependencies[j]]);
            }
        }

        while (m_NumVerts > 0)
        {
            const int currentVertex = GetLastNode();
            if (currentVertex == -1)
            {
                SPARK_ASSERT(false);
                return { };
            }

            m_SortedArray.emplace_back(systemList[m_Vertices[currentVertex]]);

            DeleteVert(currentVertex);
        }

        return m_SortedArray;
    }

    int SystemSorter::GetLastNode() const
    {
        for (int row = 0; row < m_NumVerts; row++)
        {
            bool isEdge = false;
            for (int col = 0; col < m_NumVerts; col++)
            {
                if (m_Matrix[row][col] > 0)
                {
                    isEdge = true;
                    break;
                }
            }
            if (!isEdge)
            {
                return row;
            }
        }
        return -1;
    }

    void SystemSorter::DeleteVert(const int vert)
    {
        if (vert != m_NumVerts - 1)
        {
            for (int j = vert; j < m_NumVerts - 1; j++)
            {
                m_Vertices[j] = m_Vertices[j + 1];
            }

            for (int row = vert; row < m_NumVerts - 1; row++)
            {
                MoveRowUp(row, m_NumVerts);
            }

            for (int col = vert; col < m_NumVerts - 1; col++)
            {
                MoveColumnLeft(col, m_NumVerts - 1);
            }
        }
        m_NumVerts--;
    }

    void SystemSorter::MoveRowUp(const int row,
        const int length)
    {
        for (int col = 0; col < length; col++)
        {
            m_Matrix[row][col] = m_Matrix[row + 1][col];
        }
    }

    void SystemSorter::MoveColumnLeft(const int col,
        const int length)
    {
        for (int row = 0; row < length; row++)
        {
            m_Matrix[row][col] = m_Matrix[row][col + 1];
        }
    }
}
