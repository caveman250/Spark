#include "spark.h"
#include "InputComponent.h"
#include "MouseButton.h"

namespace se::input
{
    DEFINE_SPARK_COMPONENT_BEGIN(InputComponent)
        DEFINE_MEMBER(keyEvents)
        DEFINE_MEMBER(keyStates)
        DEFINE_MEMBER(mouseX)
        DEFINE_MEMBER(mouseY)
        DEFINE_MEMBER(mouseButtonStates)
        DEFINE_MEMBER(mouseEvents)
    DEFINE_SPARK_COMPONENT_END(InputComponent)

    InputComponent::InputComponent()
    {
        keyStates.resize(Key::ValuesCount());
        mouseButtonStates.resize(MouseButton::ValuesCount());
    }
}