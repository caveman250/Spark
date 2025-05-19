#include "NumberEditor.h"

namespace se::editor::ui::properties
{
    DEFINE_PROPERTY_EDITOR(char, NumberEditor<char>, char);
    DEFINE_PROPERTY_EDITOR(int8_t, NumberEditor<int8_t>, int8_t);
    DEFINE_PROPERTY_EDITOR(uint8_t, NumberEditor<uint8_t>, uint8_t);
    DEFINE_PROPERTY_EDITOR(int16_t, NumberEditor<int16_t>, int16_t);
    DEFINE_PROPERTY_EDITOR(uint16_t, NumberEditor<uint16_t>, uint16_t);
    DEFINE_PROPERTY_EDITOR(int32_t, NumberEditor<int32_t>, int32_t);
    DEFINE_PROPERTY_EDITOR(uint32_t, NumberEditor<uint32_t>, uint32_t);
    DEFINE_PROPERTY_EDITOR(int64_t, NumberEditor<int64_t>, int64_t);
    DEFINE_PROPERTY_EDITOR(uint64_t, NumberEditor<uint64_t>, uint64_t);
    DEFINE_PROPERTY_EDITOR(float, NumberEditor<float>, float);
    DEFINE_PROPERTY_EDITOR(double, NumberEditor<double>, double);
}
