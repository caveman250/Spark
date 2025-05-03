#include "IntEditor.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(char, IntEditor<char>, char);
    DEFINE_PROPERTY_EDITOR(int8_t, IntEditor<int8_t>, int8_t);
    DEFINE_PROPERTY_EDITOR(uint8_t, IntEditor<uint8_t>, uint8_t);
    DEFINE_PROPERTY_EDITOR(int16_t, IntEditor<int16_t>, int16_t);
    DEFINE_PROPERTY_EDITOR(uint16_t, IntEditor<uint16_t>, uint16_t);
    DEFINE_PROPERTY_EDITOR(int32_t, IntEditor<int32_t>, int32_t);
    DEFINE_PROPERTY_EDITOR(uint32_t, IntEditor<uint32_t>, uint32_t);
    DEFINE_PROPERTY_EDITOR(int64_t, IntEditor<int64_t>, int64_t);
    DEFINE_PROPERTY_EDITOR(uint64_t, IntEditor<uint64_t>, uint64_t);
}
