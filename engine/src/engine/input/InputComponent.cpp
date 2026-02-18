#include "spark.h"
#include "InputComponent.h"
#include "MouseButton.h"

namespace se::input
{
    InputComponent::InputComponent()
    {
        const reflect::Enum* key = reflect::EnumResolver<Key>::Get();
        const reflect::Enum* mouseButton = reflect::EnumResolver<MouseButton>::Get();
        keyStates.resize(key->values.size());
        mouseButtonStates.resize(mouseButton->values.size());
    }
}