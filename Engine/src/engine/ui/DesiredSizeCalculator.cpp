#include "DesiredSizeCalculator.h"
#include "Widgets.generated.h"

se::math::Vec2 se::ui::DesiredSizeCalculator::GetChildrenDesiredSize(se::ecs::System *,
                                                                     const ecs::Id& ,
                                                                     const se::ui::Rect&)
{
    math::Vec2 ret = {};
    //TODO
//    auto declaration = ecs::ChildQueryDeclaration()
//        .WithVariantComponent<std::variant<SPARK_CONST_WIDGET_TYPES>>(ecs::ComponentMutability::Immutable);
//    system->RunChildQuery(entity, declaration,
//      (std::function<bool(const ecs::Id&, ConstWidgetVariant)>)[&ret, system, entity, parentRect](const ecs::SystemUpdateData& updateData)
//    {
//        std::visit([&ret, system, entity, parentRect](auto&& value)
//        {
//            // TODO calc parent rect here.
//            ret = std::max(ret, DesiredSizeCalculator::GetDesiredSize(system, entity, parentRect, value));
//        }, var);
//        return false;
//    });

    return ret;
}
