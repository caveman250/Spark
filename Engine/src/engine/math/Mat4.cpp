#include "Mat4.h"

#include "math.h"
#include "Vec3.h"

namespace se::math
{
    DEFINE_SPARK_POD_CLASS_BEGIN(Mat4)
        DEFINE_MEMBER(m_Value)
    DEFINE_SPARK_CLASS_END()

    Mat4::Mat4()
        : m_Value { Vec4(1.f, 0, 0, 0), Vec4(0, 1.f, 0, 0), Vec4(0, 0, 1.f, 0), Vec4(0, 0, 0, 1.f) }
    {
    }

    Mat4::Mat4(const Mat4& other)
    {
        *this = other;
    }

    Mat4::Mat4(float scalar)
        : m_Value { Vec4(scalar, 0, 0, 0), Vec4(0, scalar, 0, 0), Vec4(0, 0, scalar, 0), Vec4(0, 0, 0, scalar) }
    {
    }

    Mat4::Mat4(float x0, float y0, float z0, float w0, float x1, float y1, float z1, float w1, float x2, float y2,
               float z2, float w2, float x3, float y3, float z3, float w3)
                   : m_Value { Vec4(x0, y0, z0, w0), Vec4(x1, y1, z1, w1), Vec4(x2, y2, z2, w2), Vec4(x3, y3, z3, w3) }
    {
    }

    Mat4::Mat4(const Vec4& v0, const Vec4& v1, const Vec4& v2, const Vec4& v3)
        : m_Value { v0, v1, v2, v3 }
    {
    }

    Vec4& Mat4::operator[](size_t i)
    {
        return m_Value[i];
    }

    const Vec4& Mat4::operator[](size_t i) const
    {
        return m_Value[i];
    }

    Mat4& Mat4::operator=(const Mat4& m)
    {
        if (this == &m)
            return *this;

        m_Value[0] = m[0];
        m_Value[1] = m[1];
        m_Value[2] = m[2];
        m_Value[3] = m[3];
        return *this;
    }

    Mat4& Mat4::operator+=(const Mat4& m)
    {
        m_Value[0] += m[0];
        m_Value[1] += m[1];
        m_Value[2] += m[2];
        m_Value[3] += m[3];
        return *this;
    }

    Mat4& Mat4::operator-=(const Mat4& m)
    {
        m_Value[0] -= m[0];
        m_Value[1] -= m[1];
        m_Value[2] -= m[2];
        m_Value[3] -= m[3];
        return *this;
    }

    Mat4& Mat4::operator*=(const Mat4& m)
    {
        return (*this = *this * m);
    }

    Mat4& Mat4::operator*=(float scalar)
    {
        m_Value[0] *= scalar;
        m_Value[1] *= scalar;
        m_Value[2] *= scalar;
        m_Value[3] *= scalar;
        return *this;
    }

    Mat4& Mat4::operator/=(const Mat4& m)
    {
        return *this *= Inverse(m);
    }

    Mat4& Mat4::operator/=(float scalar)
    {
        m_Value[0] /= scalar;
        m_Value[1] /= scalar;
        m_Value[2] /= scalar;
        m_Value[3] /= scalar;
        return *this;
    }

    Mat4 operator+(const Mat4& lhs, const Mat4& rhs)
    {
        Mat4 temp(lhs);
        return temp += rhs;
    }

    Mat4 operator-(const Mat4& lhs, const Mat4& rhs)
    {
        Mat4 temp(lhs);
        return temp -= rhs;
    }

    Mat4 operator*(const Mat4& lhs, float scalar)
    {
        Mat4 temp(lhs);
        return temp *= scalar;
    }

    Mat4 operator/(const Mat4& lhs, const Mat4& rhs)
    {
        Mat4 temp(lhs);
        return temp /= rhs;
    }

    Mat4 operator/(const Mat4& lhs, float scalar)
    {
        Mat4 temp(lhs);
        return temp /= scalar;
    }

    Mat4 operator-(const Mat4& m)
    {
        return { -m[0], -m[1], -m[2], -m[3] };
    }

    Mat4 operator*(const Mat4& m1, const Mat4& m2)
    {
        const Vec4& a0 = m1[0];
        const Vec4& a1 = m1[1];
        const Vec4& a2 = m1[2];
        const Vec4& a3 = m1[3];

        const Vec4& b0 = m2[0];
        const Vec4& b1 = m2[1];
        const Vec4& b2 = m2[2];
        const Vec4& b3 = m2[3];

        Mat4 ret;
        ret[0] = a3 * b0.w + a2 * b0.z + a1 * b0.y + a0 * b0.x;
        ret[1] = a3 * b1.w + a2 * b1.z + a1 * b1.y + a0 * b1.x;
        ret[2] = a3 * b2.w + a2 * b2.z + a1 * b2.y + a0 * b2.x;
        ret[3] = a3 * b3.w + a2 * b3.z + a1 * b3.y + a0 * b3.x;

        return ret;
    }

    bool operator==(const Mat4& m1, const Mat4& m2)
    {
        return (m1[0] == m2[0]) && (m1[1] == m2[1]) && (m1[2] == m2[2]) && (m1[3] == m2[3]);
    }

    bool operator!=(const Mat4& m1, const Mat4& m2)
    {
        return (m1[0] != m2[0]) || (m1[1] != m2[1]) || (m1[2] != m2[2]) || (m1[3] != m2[3]);
    }

    Mat4 Inverse(const Mat4& m)
    {
        float co00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
        float co02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
        float co03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

        float co04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
        float co06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
        float co07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

        float co08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
        float co10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
        float co11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

        float co12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
        float co14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
        float co15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

        float co16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
        float co17 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
        float co19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

        float co20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
        float co22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
        float co23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

        Vec4 f0(co00, co00, co02, co03);
        Vec4 f1(co04, co04, co06, co07);
        Vec4 f2(co08, co08, co10, co11);
        Vec4 f3(co12, co12, co14, co15);
        Vec4 f4(co16, co16, co17, co19);
        Vec4 f5(co20, co20, co22, co23);

        Vec4 v0(m[1][0], m[0][0], m[0][0], m[0][0]);
        Vec4 v1(m[1][1], m[0][1], m[0][1], m[0][1]);
        Vec4 v2(m[1][2], m[0][2], m[0][2], m[0][2]);
        Vec4 v3(m[1][3], m[0][3], m[0][3], m[0][3]);

        Vec4 inv0(v1 * f0 - v2 * f1 + v3 * f2);
        Vec4 inv1(v0 * f0 - v2 * f3 + v3 * f4);
        Vec4 inv2(v0 * f1 - v1 * f3 + v3 * f5);
        Vec4 iv3(v0 * f2 - v1 * f4 + v2 * f5);

        Vec4 signA(+1, -1, +1, -1);
        Vec4 signB(-1, +1, -1, +1);
        Mat4 inv(inv0 * signA, inv1 * signB, inv2 * signA, iv3 * signB);

        Vec4 r0(inv[0][0], inv[1][0], inv[2][0], inv[3][0]);

        Vec4 dot0(m[0] * r0);
        float dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

        float oneOverDeterminant = static_cast<float>(1) / dot1;

        return inv * oneOverDeterminant;
    }

    Mat4 Perspective(float fovy, float aspect, float zNear, float zFar)
    {
        SPARK_ASSERT(!FloatEqual(aspect, 0.f));

        float const tanHalfFovy = tan(fovy / 2.f);

        Mat4 ret(0.f);
        ret[0][0] = 1.f / (aspect * tanHalfFovy);
        ret[1][1] = 1.f / (tanHalfFovy);
        ret[2][2] = - (zFar + zNear) / (zFar - zNear);
        ret[2][3] = - 1.f;
        ret[3][2] = - (2.f * zFar * zNear) / (zFar - zNear);
        return ret;
    }

    Mat4 LookAt(const Vec3& eye, const Vec3& center, const Vec3& up)
    {
        const Vec3 f(Normalized(center - eye));
        const Vec3 s(Normalized(Cross(f, up)));
        const Vec3 u(Cross(s, f));

        Mat4 ret(1);
        ret[0][0] = s.x;
        ret[1][0] = s.y;
        ret[2][0] = s.z;
        ret[0][1] = u.x;
        ret[1][1] = u.y;
        ret[2][1] = u.z;
        ret[0][2] = -f.x;
        ret[1][2] = -f.y;
        ret[2][2] = -f.z;
        ret[3][0] = -Dot(s, eye);
        ret[3][1] = -Dot(u, eye);
        ret[3][2] = Dot(f, eye);
        return ret;
        return ret;
    }

    Mat4 Translation(const Vec3& v)
    {
        Mat4 ret;
        ret[3] = ret[0] * v[0] + ret[1] * v[1] + ret[2] * v[2] + ret[3];
        return ret;
    }

    Mat4 Scale(const Vec3& scale)
    {
        return { scale.x,    0.f,           0.f,        0.f,
                   0.f,         scale.y,        0.f,        0.f,
                   0.f,         0.f,            scale.z,    0.f,
                   0.f,         0.f,            0.f,        1.f };
    }
}
