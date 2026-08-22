export module Spark.Math;
export import :IntVec2;
export import :Vec2;
export import :Vec3;
export import :Vec4;
export import :Mat3;
export import :Mat4;

#ifndef M_PI
export constexpr float M_PI = 3.14159265358979323846;
#endif

namespace se::math
{
    export bool FloatEqual(float a, float b);
    export float Radians(float degrees);
    export float Degrees(float radians);
    export float InverseSqrt(float f);
    export Mat4 AxisAngle(Vec3 axis, float angle);
    export Vec3 EulerFromMat4(const Mat4& mat);
}