#pragma once

#include "spark.h"
#include "engine/math/math.h"


namespace se::ecs
{
    class World;
    class System;
}

namespace se::asset
{
    class Font;
}

namespace se::ui::components
{
    struct KeyInputComponent;
    struct EditableTextComponent;
    struct RectTransformComponent;
}

namespace se::ui::util
{
    ecs::Id CreateEditableText(ecs::World* world,
                               const std::shared_ptr<asset::Font>& font,
                               int fontSize,
                               components::EditableTextComponent** text);

    math::IntVec2 GetCaretPosition(int pos,
                                   const components::EditableTextComponent& text,
                                   const components::RectTransformComponent& rect);

    int CalcCaretPosition(const math::Vec2& mousePos,
                                   const components::EditableTextComponent& text,
                                   const components::RectTransformComponent& rect);

    void BeginEditingText(ecs::System* system,
                          const ecs::Id& entity,
                          components::EditableTextComponent& textComp,
                          components::KeyInputComponent& keyInputComp);

    void EndEditingText(ecs::System* system,
                        const ecs::Id& entity,
                        components::EditableTextComponent& textComp,
                        components::KeyInputComponent& keyInputComp);

    void MoveCaret(components::EditableTextComponent& textComp,
                   int delta);

    void SetCaretPos(components::EditableTextComponent& textComp,
                     int pos);
}
