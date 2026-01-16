#pragma once

#include "spark.h"
#include "Widgets.generated.h"

namespace se::ecs
{
    class System;
    struct HeirachyQueryDeclaration;
    class SystemUpdateData;
}

namespace se::ui::util
{
    template <typename Func>
    void ForEachWidgetChild(const ecs::Id& entity,
                            ecs::System* system,
                            const ecs::HeirachyQueryDeclaration& query,
                            Func&& func);

    template<typename Func>
    void ForEachWidgetChild(const ecs::Id& entity,
        ecs::System* system,
        const ecs::HeirachyQueryDeclaration& query,
        Func&& func)
    {
        ecs::HeirachyQueryDeclaration decCopy = query;
        std::ignore = decCopy.WithVariantComponent<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>(ecs::ComponentMutability::Immutable);

        system->RunChildQuery(entity, decCopy, [func](const ecs::SystemUpdateData& updateData)
        {
            std::visit([updateData, func](auto&& value)
            {
                func(updateData, value);
            }, updateData.GetVariantComponentArray<SPARK_CONST_WIDGET_TYPES_WITH_NULLTYPE>());

            return false;
        });
    }

    void ForEachWidgetChildInternal(const ecs::Id& entity,
                            ecs::System* system,
                            const ecs::HeirachyQueryDeclaration& query,
                            const std::function<void(ecs::SystemUpdateData)>& func);
}
