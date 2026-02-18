#pragma once

#include "spark.h"
#include "engine/math/math.h"
#include "engine/asset/AssetReference.h"

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
    struct MouseInputComponent;
    struct WidgetComponent;
}

namespace se::ui::util
{
#if SPARK_EDITOR
    struct NewEditableText
    {
        ecs::Id entity = {};
        components::EditableTextComponent* text = nullptr;
        components::WidgetComponent* widget = nullptr;
        components::MouseInputComponent* mouseInput = nullptr;
    };
    NewEditableText CreateEditableText(ecs::World* world,
                               const asset::AssetReference<asset::Font>& font,
                               int fontSize);

    void SetEditTextMouseInputEnabled(components::MouseInputComponent* mouseInput, bool enabled);

    float GetCaretPosition(int pos, const components::EditableTextComponent& text);

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
#endif
}
