#include "spark.h"
#include "InputComponent.h"
#include "MouseButton.h"

namespace se::input
{
    InputComponent::InputComponent()
    {
        keyStates.resize(Key::ValuesCount());
        mouseButtonStates.resize(MouseButton::ValuesCount());
    }
}