#include "Mat3.h"

namespace se::math
{
    Mat3::Mat3()
    : m_Value { Vec3(1.f, 0, 0), Vec3(0, 1.f, 0), Vec3(0, 0, 1.f) }
    {
    }

    Mat3::Mat3(const Mat3& other)
    {
        *this = other;
    }

    Mat3::Mat3(const float scalar)
        : m_Value { Vec3(scalar, 0, 0), Vec3(0, scalar, 0), Vec3(0, 0, scalar) }
    {
    }

    Mat3::Mat3(const float x0, const float y0, const float z0,
                 const float x1, const float y1, const float z1,
                 const float x2, const float y2, const float z2)
        : m_Value { Vec3(x0, y0, z0), Vec3(x1, y1, z1), Vec3(x2, y2, z2) }
    {
    }

    Mat3::Mat3(const Vec3& v0, const Vec3& v1, const Vec3& v2)
        : m_Value { v0, v1, v2 }
    {
    }

    Vec3& Mat3::operator[](const size_t i)
    {
        return m_Value[i];
    }

    const Vec3& Mat3::operator[](const size_t i) const
    {
        return m_Value[i];
    }

    Mat3& Mat3::operator=(const Mat3& m)
    {
        if (this == &m)
            return *this;

        m_Value[0] = m[0];
        m_Value[1] = m[1];
        m_Value[2] = m[2];
        return *this;
    }

    Mat3& Mat3::operator+=(const Mat3& m)
    {
        m_Value[0] += m[0];
        m_Value[1] += m[1];
        m_Value[2] += m[2];
        return *this;
    }

    Mat3& Mat3::operator-=(const Mat3& m)
    {
        m_Value[0] -= m[0];
        m_Value[1] -= m[1];
        m_Value[2] -= m[2];
        return *this;
    }

    Mat3& Mat3::operator*=(const Mat3& m)
    {
        return *this = *this * m;
    }

    Mat3& Mat3::operator*=(const float scalar)
    {
        m_Value[0] *= scalar;
        m_Value[1] *= scalar;
        m_Value[2] *= scalar;
        return *this;
    }

    Mat3& Mat3::operator/=(const Mat3& m)
    {
        return *this *= Inverse(m);
    }

    Mat3& Mat3::operator/=(const float scalar)
    {
        m_Value[0] /= scalar;
        m_Value[1] /= scalar;
        m_Value[2] /= scalar;
        return *this;
    }

    Mat3 operator+(const Mat3& lhs, const Mat3& rhs)
    {
        Mat3 temp(lhs);
        return temp += rhs;
    }

    Mat3 operator-(const Mat3& lhs, const Mat3& rhs)
    {
        Mat3 temp(lhs);
        return temp -= rhs;
    }

    Mat3 operator*(const Mat3& lhs, const float scalar)
    {
        Mat3 temp(lhs);
        return temp *= scalar;
    }

    Mat3 operator/(const Mat3& lhs, const Mat3& rhs)
    {
        Mat3 temp(lhs);
        return temp /= rhs;
    }

    Mat3 operator/(const Mat3& lhs, const float scalar)
    {
        Mat3 temp(lhs);
        return temp /= scalar;
    }

    Mat3 operator-(const Mat3& m)
    {
        return { -m[0], -m[1], -m[2] };
    }

    Mat3 operator*(const Mat3& lhs, const Mat3& rhs)
    {
        const Vec3& a0 = lhs[0];
        const Vec3& a1 = lhs[1];
        const Vec3& a2 = lhs[2];

        const Vec3& b0 = rhs[0];
        const Vec3& b1 = rhs[1];
        const Vec3& b2 = rhs[2];

        Mat3 Result;
        Result[0] = a2 * b0.z + a1 * b0.y + a0 * b0.x;
        Result[1] = a2 * b1.z + a1 * b1.y + a0 * b1.x;
        Result[2] = a2 * b2.z + a1 * b2.y + a0 * b2.x;
        return Result;
    }

    bool operator==(const Mat3& lhs, const Mat3& rhs)
    {
        return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2];
    }

    bool operator!=(const Mat3& lhs, const Mat3& rhs)
    {
        return (lhs[0] != rhs[0]) || (lhs[1] != rhs[1]) || (lhs[2] != rhs[2]);
    }

    Mat3 Inverse(const Mat3& m)
    {
        const float oneOverDeterminant = 1.f / (
        +m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
        - m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
        + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]));

        Mat3 inverse;
        inverse[0][0] = +(m[1][1] * m[2][2] - m[2][1] * m[1][2]);
        inverse[1][0] = -(m[1][0] * m[2][2] - m[2][0] * m[1][2]);
        inverse[2][0] = +(m[1][0] * m[2][1] - m[2][0] * m[1][1]);
        inverse[0][1] = -(m[0][1] * m[2][2] - m[2][1] * m[0][2]);
        inverse[1][1] = +(m[0][0] * m[2][2] - m[2][0] * m[0][2]);
        inverse[2][1] = -(m[0][0] * m[2][1] - m[2][0] * m[0][1]);
        inverse[0][2] = +(m[0][1] * m[1][2] - m[1][1] * m[0][2]);
        inverse[1][2] = -(m[0][0] * m[1][2] - m[1][0] * m[0][2]);
        inverse[2][2] = +(m[0][0] * m[1][1] - m[1][0] * m[0][1]);

        inverse *= oneOverDeterminant;
        return inverse;
    }
}
