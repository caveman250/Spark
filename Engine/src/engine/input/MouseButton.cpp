#include "spark.h"
#include "MouseButton.h"

DEFINE_SPARK_ENUM_BEGIN(se::input::MouseButton)
    DEFINE_ENUM_VALUE(MouseButton, Unknown)
    DEFINE_ENUM_VALUE(MouseButton, Left)
    DEFINE_ENUM_VALUE(MouseButton, Right)
    DEFINE_ENUM_VALUE(MouseButton, Middle)
    DEFINE_ENUM_VALUE(MouseButton, Btn4)
    DEFINE_ENUM_VALUE(MouseButton, Btn5)
    DEFINE_ENUM_VALUE(MouseButton, None)
DEFINE_SPARK_ENUM_END()