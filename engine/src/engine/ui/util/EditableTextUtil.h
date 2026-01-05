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
#if SPARK_EDITOR
    ecs::Id CreateEditableText(ecs::World* world,
                               const std::shared_ptr<asset::Font>& font,
                               int fontSize,
                               EditableTextComponent** text);

    math::IntVec2 GetCaretPosition(int pos,
                                   const EditableTextComponent& text,
                                   const RectTransformComponent& rect);

    int CalcCaretPosition(const math::Vec2& mousePos,
                                   const EditableTextComponent& text,
                                   const RectTransformComponent& rect);

    void BeginEditingText(ecs::System* system,
                          const ecs::Id& entity,
                          EditableTextComponent& textComp,
                          KeyInputComponent& keyInputComp);

    void EndEditingText(ecs::System* system,
                        const ecs::Id& entity,
                        EditableTextComponent& textComp,
                        KeyInputComponent& keyInputComp);

    void MoveCaret(EditableTextComponent& textComp,
                   int delta);

    void SetCaretPos(EditableTextComponent& textComp,
                     int pos);
#endif
}
