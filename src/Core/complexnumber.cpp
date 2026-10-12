#include "complexnumber.h"
#include <cmath>

// ============================================================================
// Арифметика
// ============================================================================

ComplexNumber ComplexNumber::operator+(const ComplexNumber& other) const
{
    return ComplexNumber(m_re + other.m_re, m_im + other.m_im);
}

ComplexNumber ComplexNumber::operator-(const ComplexNumber& other) const
{
    return ComplexNumber(m_re - other.m_re, m_im - other.m_im);
}

ComplexNumber ComplexNumber::operator-() const
{
    return ComplexNumber(-m_re, -m_im);
}

ComplexNumber ComplexNumber::operator*(const ComplexNumber& other) const
{
    // (a + bi)(c + di) = (ac - bd) + (ad + bc)i
    return ComplexNumber(
        m_re * other.m_re - m_im * other.m_im,
        m_re * other.m_im + m_im * other.m_re
        );
}

ComplexNumber ComplexNumber::operator/(const ComplexNumber& other) const
{
    // (a + bi) / (c + di) = ((ac + bd) + (bc - ad)i) / (c² + d²)
    BigFloat denom = other.m_re * other.m_re + other.m_im * other.m_im;
    return ComplexNumber(
        (m_re * other.m_re + m_im * other.m_im) / denom,
        (m_im * other.m_re - m_re * other.m_im) / denom
        );
}

// ============================================================================
// Сравнение
// ============================================================================

bool ComplexNumber::operator==(const ComplexNumber& other) const
{
    return m_re == other.m_re && m_im == other.m_im;
}

// ============================================================================
// Математика
// ============================================================================

ComplexNumber ComplexNumber::conjugate() const
{
    return ComplexNumber(m_re, -m_im);
}

BigFloat ComplexNumber::norm() const
{
    return m_re * m_re + m_im * m_im;
}

BigFloat ComplexNumber::abs() const
{
    return BigFloat(std::sqrt(norm().toDouble()));
}

// ============================================================================
// Форматирование
// ============================================================================

QString ComplexNumber::toString() const
{
    if (m_im.isZero()) return m_re.toString();

    QString imStr = (m_im.isOne())      ? "i"  :
                        (m_im.isMinusOne()) ? "-i" :
                        m_im.toString() + "i";

    if (m_re.isZero()) return imStr;

    return m_re.toString() + (m_im.isNegative() ? "" : "+") + imStr;
}
