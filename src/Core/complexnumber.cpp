#include "complexnumber.h"

QString ComplexNumber::toString() const{
    if (im.isZero()) return re.toString();

    QString imStr = (im.isOne())      ? "i"  :
                    (im.isMinusOne()) ? "-i" :
                    im.toString() + "i";

    if (re.isZero()) return imStr;

    return re.toString() + (im.isNegative() ? "" : "+") + imStr;
}
