#ifndef BIGFLOAT_H
#define BIGFLOAT_H

#include <QString>
#include <cstdint>

class BigFloat {
private:
    double  m_mantissa;   // |m| ∈ [1, 10) либо 0
    int64_t m_exponent;   // число = m_mantissa * 10^m_exponent

    void normalize();
    void negate() { m_mantissa = -m_mantissa; }

public:
    BigFloat() : m_mantissa(0.0), m_exponent(0) {}
    BigFloat(double value);
    BigFloat(double m, int64_t e) : m_mantissa(m), m_exponent(e) { normalize(); }

    // Арифметика
    BigFloat operator+(const BigFloat& other) const;
    BigFloat operator-() const;
    BigFloat operator-(const BigFloat& other) const { return *this + (-other); }
    BigFloat operator*(const BigFloat& other) const;
    BigFloat operator/(const BigFloat& other) const;

    BigFloat& operator+=(const BigFloat& other) { *this = *this + other; return *this; }
    BigFloat& operator-=(const BigFloat& other) { *this = *this - other; return *this; }
    BigFloat& operator*=(const BigFloat& other) { *this = *this * other; return *this; }
    BigFloat& operator/=(const BigFloat& other) { *this = *this / other; return *this; }

    // Сравнение
    bool operator<(const BigFloat& other) const;
    bool operator>(const BigFloat& other) const  { return other < *this; }
    bool operator<=(const BigFloat& other) const { return !(other < *this); }
    bool operator>=(const BigFloat& other) const { return !(*this < other); }
    bool operator==(const BigFloat& other) const;
    bool operator!=(const BigFloat& other) const { return !(*this == other); }

    // Форматирование
    QString toString(int maxWidth = -1) const;
    QString toScientific() const;
    QString toDecimal() const;

    // Утилиты
    bool   isZero()   const { return m_mantissa == 0.0; }
    bool   isNegative() const { return m_mantissa < 0.0; }
    double toDouble() const;
    BigFloat abs() const;

    double  mantissa() const { return m_mantissa; }
    int64_t exponent() const { return m_exponent; }

    // Порог, при котором double перестаёт давать значащие цифры
    static constexpr int64_t kMaxExponent = 308;
    static constexpr int64_t kMinExponent = -324;
    static constexpr int     kSignificantDigits = 15;
};

#endif // BIGFLOAT_H
