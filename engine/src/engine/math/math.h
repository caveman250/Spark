#pragma once
#include "IntVec2.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Mat3.h"
#include "Mat4.h"

namespace se::math
{
    bool FloatEqual(float a, float b);
    float Radians(float degrees);
    float Degrees(float radians);
    float InverseSqrt(float f);
    Mat4 AxisAngle(Vec3 axis, float angle);
    Vec3 EularFromMat4(const Mat4& mat);
}
