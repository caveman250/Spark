#include "StringEditor.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(std::string, StringEditor<std::string>, string);
    DEFINE_PROPERTY_EDITOR(string::Symbol, StringEditor<string::Symbol>, Symbol);
}