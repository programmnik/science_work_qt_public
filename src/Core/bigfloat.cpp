#include "bigfloat.h"
#include <cmath>
#include <algorithm>

BigFloat::BigFloat(double value)
    : m_mantissa(value)
    , m_exponent(0)
{
    normalize();
}

// Нормализация
void BigFloat::normalize()
{
    if (m_mantissa == 0.0 || std::isnan(m_mantissa)) {
        m_mantissa = 0.0;
        m_exponent = 0;
        return;
    }

    if (std::isinf(m_mantissa)) {
        // Бесконечность — оставляем как есть, экспонента = 0
        m_exponent = 0;
        return;
    }

    double a = std::abs(m_mantissa);

    // Приводим |mantissa| к [1, 10)
    if (a >= 10.0 || a < 1.0) {
        int64_t e10 = static_cast<int64_t>(std::floor(std::log10(a)));
        m_mantissa /= std::pow(10.0, static_cast<double>(e10));
        m_exponent += e10;

        // Компенсация погрешностей log10/pow
        while (std::abs(m_mantissa) >= 10.0) { m_mantissa /= 10.0; m_exponent += 1; }
        while (std::abs(m_mantissa) < 1.0 && m_mantissa != 0.0) { m_mantissa *= 10.0; m_exponent -= 1; }
    }
}

BigFloat BigFloat::operator+(const BigFloat& other) const
{
    if (isZero())       return other;
    if (other.isZero()) return *this;

    int64_t dExp = m_exponent - other.m_exponent;

    // Разница настолько велика, что меньшее не влияет на double
    if (dExp >  kSignificantDigits) return *this;
    if (dExp < -kSignificantDigits) return other;

    // Работаем в системе большего экспонента, чтобы не терять точность
    int64_t baseExp = std::max(m_exponent, other.m_exponent);

    double m1 = m_mantissa        * std::pow(10.0, static_cast<double>(m_exponent        - baseExp));
    double m2 = other.m_mantissa  * std::pow(10.0, static_cast<double>(other.m_exponent  - baseExp));

    double sum = m1 + m2;
    if (sum == 0.0) return BigFloat();

    // Нормализуем вручную, т.к. конструктор требует |m|>=1 для корректности
    return BigFloat(sum, baseExp);
}

BigFloat BigFloat::operator-() const {
    BigFloat result(*this);
    result.negate();
    return result;
}

BigFloat BigFloat::operator*(const BigFloat& other) const
{
    if (isZero() || other.isZero()) return BigFloat();

    double m = m_mantissa * other.m_mantissa;
    int64_t e = m_exponent + other.m_exponent;
    return BigFloat(m, e);
}

BigFloat BigFloat::operator/(const BigFloat& other) const
{
    if (other.isZero()) {
        // Деление на ноль — возвращаем inf-представление
        return BigFloat(std::numeric_limits<double>::infinity() *
                            (m_mantissa < 0 ? -1.0 : 1.0), 0);
    }
    if (isZero()) return BigFloat();

    double m = m_mantissa / other.m_mantissa;
    int64_t e = m_exponent - other.m_exponent;
    return BigFloat(m, e);
}

bool BigFloat::operator<(const BigFloat& other) const
{
    bool neg1 = m_mantissa < 0.0;
    bool neg2 = other.m_mantissa < 0.0;

    if (neg1 != neg2) return neg1;
    if (isZero() && other.isZero()) return false;

    bool result;
    if (m_exponent != other.m_exponent) {
        result = m_exponent < other.m_exponent;
    } else {
        result = std::abs(m_mantissa) < std::abs(other.m_mantissa);
    }

    return neg1 ? !result : result;
}

bool BigFloat::operator==(const BigFloat& other) const
{
    if (isZero() && other.isZero()) return true;
    return m_exponent == other.m_exponent
           && m_mantissa == other.m_mantissa;
}

double BigFloat::toDouble() const
{
    if (isZero()) return 0.0;

    // Если экспонента выходит за пределы double — насыщаем
    if (m_exponent >  kMaxExponent) return  std::numeric_limits<double>::infinity() * (m_mantissa < 0 ? -1.0 : 1.0);
    if (m_exponent <  kMinExponent) return  0.0;

    return m_mantissa * std::pow(10.0, static_cast<double>(m_exponent));
}

BigFloat BigFloat::abs() const
{
    BigFloat r = *this;
    r.m_mantissa = std::abs(r.m_mantissa);
    return r;
}

QString BigFloat::toScientific() const
{
    if (isZero()) return QStringLiteral("0");

    // Формат: 1.2345e-10
    // Убираем лишние нули в конце мантиссы
    QString mStr = QString::number(m_mantissa, 'g', kSignificantDigits);

    // QString::number с 'g' может сам выдать "1.23e-10" — тогда норм
    // Но нам нужно гарантировать вид <mantissa>e<exponent>
    if (mStr.contains('e') || mStr.contains('E')) {
        return mStr;
    }

    return mStr + "e" + QString::number(m_exponent);
}

QString BigFloat::toDecimal() const
{
    if (isZero()) return QStringLiteral("0");

    // Если число умещается в разумный диапазон — печатаем позиционно
    // Иначе переходим в научный формат
    if (m_exponent > 15 || m_exponent < -15) {
        return toScientific();
    }

    double value = toDouble();

    // Определяем, сколько знаков после запятой нужно
    int decimals = std::max(0, kSignificantDigits - 1 - static_cast<int>(m_exponent));

    QString s = QString::number(value, 'f', decimals);

    // Убираем хвостовые нули
    if (s.contains('.')) {
        while (s.endsWith('0')) s.chop(1);
        if (s.endsWith('.')) s.chop(1);
    }
    return s;
}

QString BigFloat::toString(int maxWidth) const
{
    if (isZero()) return QStringLiteral("0");

    QString decimal    = toDecimal();
    QString scientific = toScientific();

    if (maxWidth < 0) {
        // Без ограничения: выбираем "естественную" форму
        // Если позиционное представление короткое — используем его
        return decimal.length() <= scientific.length() ? decimal : scientific;
    }

    // Пытаемся влезть в ширину
    if (decimal.length() <= maxWidth)    return decimal;
    if (scientific.length() <= maxWidth) return scientific;

    // Не влезло — урезаем научный формат
    return scientific.left(maxWidth - 1) + QChar(0x2026); // …
}
