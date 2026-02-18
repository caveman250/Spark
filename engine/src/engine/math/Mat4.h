#pragma once

#include "Vec4.h"

namespace se::math
{
    struct Vec3;

    class Mat4
    {
        SPARK_POD_CLASS()

    public:
        Mat4();
        Mat4(const Mat4& other);
        explicit Mat4(float scalar);
        Mat4(float x0, float y0, float z0, float w0,
            float x1, float y1, float z1, float w1,
            float x2, float y2, float z2, float w2,
            float x3, float y3, float z3, float w3);

        Mat4(const Vec4& v0,
            const Vec4& v1,
            const Vec4& v2,
            const Vec4& v3);

        Vec4& operator[](size_t i);
        const Vec4& operator[](size_t i) const;

        Mat4& operator=(Mat4& m);
        Mat4& operator=(const Mat4& m);
        Mat4& operator+=(const Mat4& m);
        Mat4& operator-=(const Mat4& m);
        Mat4& operator*=(const Mat4& m);
        Mat4& operator*=(float scalar);
        Mat4& operator/=(const Mat4& m);
        Mat4& operator/=(float scalar);

    private:
        SPARK_MEMBER(Serialized)
        std::array<Vec4, 4> m_Value;
    };
    
    Mat4 operator+(const Mat4& lhs, const Mat4 &rhs);
    Mat4 operator-(const Mat4& lhs, const Mat4 &rhs);
    Mat4 operator*(const Mat4& lhs, const Mat4 &rhs);
    Mat4 operator*(const Mat4& lhs, float scalar);
    Vec4 operator*(const Mat4& lhs, const Vec4& rhs);
    Mat4 operator/(const Mat4& lhs, const Mat4 &rhs);
    Mat4 operator/(const Mat4& lhs, float scalar);
    bool operator==(const Mat4& lhs, const Mat4 &rhs);
    bool operator!=(const Mat4& lhs, const Mat4 &rhs);

    Mat4 Inverse(const Mat4& m);

    Mat4 Perspective(float fovY, float aspect, float zNear, float zFar);
    Mat4 LookAt(const Vec3& eye, const Vec3& center, const Vec3& up);
    Mat4 Translation(const Vec3& translation);
    Mat4 Scale(const Vec3& scale);
    Vec3 UnProject(const Vec3& windowPos, const Mat4& view, const Mat4& proj, const Vec4& viewport);
}
