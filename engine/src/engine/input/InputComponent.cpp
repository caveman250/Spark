#include "spark.h"
#include "InputComponent.h"
#include "MouseButton.h"

namespace se::input
{
    InputComponent::InputComponent()
    {
        reflect::Enum* key = reflect::EnumResolver<Key>::get();
        reflect::Enum* mouseButton = reflect::EnumResolver<MouseButton>::get();
        keyStates.resize(key->values.size());
        mouseButtonStates.resize(mouseButton->values.size());
    }
}