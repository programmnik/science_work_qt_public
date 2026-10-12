#ifndef COMPLEXNUMBER_H
#define COMPLEXNUMBER_H

#include <QString>
#include "bigfloat.h"

/**
 * ComplexNumber — комплексное число re + i·im.
 *
 * Значения re и im — произвольные BigFloat.
 * Специальные состояния:
 *   - isZero()    — обе части нулевые
 *   - isOnAxis()  — вещественная или мнимая часть нулевая
 */
class ComplexNumber
{
public:
    // === Создание ===

    ComplexNumber()
        : m_re(BigFloat(0.0))
        , m_im(BigFloat(0.0))
    {}

    ComplexNumber(const BigFloat& re, const BigFloat& im)
        : m_re(re), m_im(im)
    {}

    ComplexNumber(const ComplexNumber&) = default;
    ComplexNumber& operator=(const ComplexNumber&) = default;
    ~ComplexNumber() = default;

    // === Состояние ===

    bool isOnAxis() const { return m_re.isZero() || m_im.isZero(); }
    bool isZero()   const { return m_re.isZero() && m_im.isZero(); }

    // === Доступ к компонентам ===

    void setRe(const BigFloat& re) { m_re = re; }
    void setIm(const BigFloat& im) { m_im = im; }

    const BigFloat& getRe() const { return m_re; }
    const BigFloat& getIm() const { return m_im; }

    // === Арифметика ===

    ComplexNumber operator+(const ComplexNumber& other) const;
    ComplexNumber operator-(const ComplexNumber& other) const;
    ComplexNumber operator-() const;
    ComplexNumber operator*(const ComplexNumber& other) const;
    ComplexNumber operator/(const ComplexNumber& other) const;

    ComplexNumber& operator+=(const ComplexNumber& other) { *this = *this + other; return *this; }
    ComplexNumber& operator-=(const ComplexNumber& other) { *this = *this - other; return *this; }
    ComplexNumber& operator*=(const ComplexNumber& other) { *this = *this * other; return *this; }
    ComplexNumber& operator/=(const ComplexNumber& other) { *this = *this / other; return *this; }

    // === Сравнение ===

    bool operator==(const ComplexNumber& other) const;
    bool operator!=(const ComplexNumber& other) const { return !(*this == other); }

    // === Математика ===

    /// Сопряжённое: re - i·im.
    ComplexNumber conjugate() const;

    /// Квадрат модуля: re² + im².
    BigFloat norm() const;

    /// Модуль: sqrt(re² + im²).
    BigFloat abs() const;

    // === Форматирование ===

    QString toString() const;

private:
    BigFloat m_re;
    BigFloat m_im;
};

#endif // COMPLEXNUMBER_H
