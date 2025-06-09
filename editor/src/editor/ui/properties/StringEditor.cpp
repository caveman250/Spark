#include "StringEditor.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(String, StringEditor<String>, String);
    DEFINE_PROPERTY_EDITOR(std::string, StringEditor<std::string>, string);
}