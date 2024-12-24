#include "spark.h"
#include "InputComponent.h"
#include "MouseButton.h"

namespace se::input
{
    DEFINE_SPARK_COMPONENT_BEGIN(InputComponent)
        DEFINE_SERIALIZED_MEMBER(keyEvents)
        DEFINE_SERIALIZED_MEMBER(keyStates)
        DEFINE_SERIALIZED_MEMBER(mouseX)
        DEFINE_SERIALIZED_MEMBER(mouseY)
        DEFINE_SERIALIZED_MEMBER(mouseButtonStates)
        DEFINE_SERIALIZED_MEMBER(mouseEvents)
    DEFINE_SPARK_COMPONENT_END()

    InputComponent::InputComponent()
    {
        keyStates.resize(Key::ValuesCount());
        mouseButtonStates.resize(MouseButton::ValuesCount());
    }
}