#include "complexnumber.h"

ComplexNumber::ComplexNumber()
    : re(re)
    , im(im)
{}

ComplexNumber::ComplexNumber(float re, float im)
    : re(re)
    , im(im)
{}

QString ComplexNumber::toString() const{
    int reInt = qRound(re);
    int imInt = qRound(im);

    if (imInt == 0) return QString::number(reInt);

    QString imStr = (imInt == 1) ? "i" :
                    (imInt == -1) ? "-i" :
                    QString::number(imInt) + "i";

    if (reInt == 0) return imStr;
    return QString::number(reInt) + (imInt > 0 ? "+" : "") + imStr;
}

bool ComplexNumber::isOnAxis() const{
    return std::abs(re) < 0.001f || std::abs(im) < 0.001f;
}

bool ComplexNumber::isZero() const{
    return std::abs(re) < 0.001f && std::abs(im) < 0.001f;
}
