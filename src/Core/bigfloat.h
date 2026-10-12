#ifndef BIGFLOAT_H
#define BIGFLOAT_H

#include <QString>
#include <cstdint>

/**
 * BigFloat — число с плавающей точкой в формате:
 *
 *     value = m_mantissa * 10^m_exponent
 *
 * где |m_mantissa| ∈ [1, 10) либо m_mantissa == 0.
 *
 * Особые значения:
 *   - Ноль:      m_mantissa == 0, m_exponent == 0
 *   - ±Infinity: m_mantissa == ±inf, m_exponent == 0
 *   - NaN:       m_mantissa == NaN, m_exponent == 0
 *
 * Публичный API:
 *   - создание (из double, из научной нотации, спец-значения),
 *   - арифметика (+, -, *, /),
 *   - сравнение,
 *   - проверка состояния (isZero, isFinite, ...),
 *   - доступ к компонентам (mantissa, exponent),
 *   - преобразования (toDouble, toString).
 */
class BigFloat {
public:
    // === Создание ===

    /// Ноль.
    BigFloat() : m_mantissa(0.0), m_exponent(0) {}

    /// Создать из double.
    BigFloat(double value);

    /// Создать из научной нотации: value = m * 10^e.
    /// Мантисса будет нормализована к диапазону [1, 10).
    BigFloat(double m, int64_t e) : m_mantissa(m), m_exponent(e) { normalize(); }

    /// Алиас для BigFloat(m, e) — для читаемости на месте вызова.
    static BigFloat fromScientific(double m, int64_t e) { return BigFloat(m, e); }

    /// Специальные значения.
    static BigFloat infinity(bool negative = false);
    static BigFloat nan();

    // === Арифметика ===

    BigFloat operator+(const BigFloat& other) const;
    BigFloat operator-() const;
    BigFloat operator-(const BigFloat& other) const { return *this + (-other); }
    BigFloat operator*(const BigFloat& other) const;
    BigFloat operator/(const BigFloat& other) const;

    BigFloat& operator+=(const BigFloat& other) { *this = *this + other; return *this; }
    BigFloat& operator-=(const BigFloat& other) { *this = *this - other; return *this; }
    BigFloat& operator*=(const BigFloat& other) { *this = *this * other; return *this; }
    BigFloat& operator/=(const BigFloat& other) { *this = *this / other; return *this; }

    // === Сравнение ===

    bool operator<(const BigFloat& other) const;
    bool operator>(const BigFloat& other) const  { return other < *this; }
    bool operator<=(const BigFloat& other) const { return !(other < *this); }
    bool operator>=(const BigFloat& other) const { return !(*this < other); }
    bool operator==(const BigFloat& other) const;
    bool operator!=(const BigFloat& other) const { return !(*this == other); }

    // === Состояние ===

    bool isZero()     const { return m_mantissa == 0.0; }
    bool isOne()      const { return m_mantissa ==  1.0 && m_exponent == 0; }
    bool isMinusOne() const { return m_mantissa == -1.0 && m_exponent == 0; }
    bool isNegative() const { return m_mantissa < 0.0; }
    bool isFinite()   const;
    bool isNaN()      const;

    // === Компоненты (часть математического API) ===

    /// Мантисса. По контракту класса |mantissa| ∈ [1, 10) либо == 0.
    double  mantissa() const { return m_mantissa; }

    /// Экспонента. Значение = mantissa * 10^exponent.
    int64_t exponent() const { return m_exponent; }

    // === Преобразования ===

    double toDouble() const;
    BigFloat abs() const;

    // === Форматирование ===

    /// Строка с ограничением по ширине. maxWidth < 0 — без ограничения.
    QString toString(int maxWidth = -1) const;

    /// Научная нотация: "1.23e-10".
    QString toScientific() const;

    /// Позиционная нотация: "0.00123" или "1234.5".
    QString toDecimal() const;

    // === Константы ===

    /// Порог, за которым double перестаёт давать значащие цифры.
    static constexpr int64_t kMaxExponent = 308;
    static constexpr int64_t kMinExponent = -324;
    static constexpr int     kSignificantDigits = 15;

private:
    // === Внутреннее представление ===
    double  m_mantissa;
    int64_t m_exponent;

    void normalize();
    void negate() { m_mantissa = -m_mantissa; }
};

#endif // BIGFLOAT_H
