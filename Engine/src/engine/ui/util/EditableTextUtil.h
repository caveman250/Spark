#pragma once

namespace se::ecs
{
    class World;
}

namespace se::ui::components
{
    struct KeyInputComponent;
    struct EditableTextComponent;
}

namespace se::ui::util
{
    ecs::Id CreateEditableText(ecs::World* world,
                               const std::shared_ptr<asset::Font>& font,
                               size_t fontSize,
                               EditableTextComponent** text);

    math::IntVec2 GetCaretPosition(int pos,
                                   const EditableTextComponent& text,
                                   const RectTransformComponent& rect);

    int CalcCaretPosition(const math::Vec2& mousePos,
                                   const EditableTextComponent& text,
                                   const RectTransformComponent& rect);

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
