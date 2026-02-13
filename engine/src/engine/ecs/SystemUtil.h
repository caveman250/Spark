#pragma once
#include "UpdateMode.h"

namespace se::ecs
{
    struct QueryArchetype;
    struct QueryResults;
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

    template <typename Func>
    void ForEachArcheType(const QueryResults& queryResults,
        UpdateMode mode,
        bool force,
        Func&& func)
    {
        EASY_BLOCK("ForEachArcheType");
        auto world = Application::Get()->GetWorld();
        auto runOnArchetype = [&queryResults, func, world](const QueryArchetype& archetype) mutable
        {
            auto updateDataCopy = queryResults.updateData;
            if (!archetype.archetype->entities.empty())
            {
                updateDataCopy.ClearEntityData();
                updateDataCopy.SetEntities(archetype.archetype->entities);
                for (const auto& compUsage: queryResults.componentUsage)
                {
                    updateDataCopy.AddComponentArray(compUsage.id, world->GetComponent(archetype.archetype->entities[0], compUsage.id), compUsage.mutability);
                }
                if (archetype.variantCompType != s_InvalidEntity)
                {
                    updateDataCopy.AddVariantComponentArray(archetype.variantCompType, world->GetComponent(archetype.archetype->entities[0], archetype.variantCompType), queryResults.variantComponent.mutability);
                }

                func(updateDataCopy);
            }
        };

        switch (mode)
        {
            case UpdateMode::SingleThreaded:
                for (const auto& archetype : queryResults.archetypes)
                {
                    runOnArchetype(archetype);
                }
                break;
            case UpdateMode::MultiThreaded:
                threads::ParallelForEach(queryResults.archetypes, runOnArchetype);
                break;
        }
        EASY_END_BLOCK

        EASY_BLOCK("Run on empty");
        bool hasStaticComps = !queryResults.singletonComponents.empty();
        if (queryResults.archetypes.empty() && (force || hasStaticComps))
        {
            auto updateDataCopy = queryResults.updateData;
            static std::vector<Id> empty = {};
            updateDataCopy.SetEntities(empty);
            for (const auto& compUsage: queryResults.componentUsage)
            {
                updateDataCopy.AddComponentArray(compUsage.id, nullptr, compUsage.mutability);
            }
            func(updateDataCopy);
        }
        EASY_END_BLOCK
    }
}
