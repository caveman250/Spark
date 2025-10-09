#pragma once

namespace se::ecs
{
    struct SystemRecord;
    template <typename T>
    concept ClassWithGetSystemDeclaration = requires
    {
        { T::GetSystemDeclaration() } -> std::same_as<SystemDeclaration>;
    };

    template <ClassWithGetSystemDeclaration T>
    constexpr bool HasGetSystemDeclaration()
    {
        return true;
    }

    template <typename T>
    constexpr bool HasGetSystemDeclaration()
    {
        return false;
    }

    class SystemSorter
    {
    public:
        std::vector<std::pair<Id, SystemRecord>> Sort(const std::unordered_map<Id, SystemRecord>& systems);

    private:
        int AddVertex(int vertex);
        void AddEdge(int start,
                     int end);
        int GetLastNode() const;
        void DeleteVert(int vert);
        void MoveRowUp(int row,
                       int length);
        void MoveColumnLeft(int col,
                            int length);

        std::vector<int> m_Vertices = { };
        std::vector<std::vector<int>> m_Matrix = { };
        int m_NumVerts = { };
        std::vector<std::pair<Id, SystemRecord>> m_SortedArray = { };
    };
}