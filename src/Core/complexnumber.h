#ifndef COMPLEXNUMBER_H
#define COMPLEXNUMBER_H

#include <QString>
#include "bigfloat.h"

class ComplexNumber
{
public:
    ComplexNumber(): re(BigFloat(0.0f)), im(BigFloat(0.0f)){}
    ComplexNumber(BigFloat re, BigFloat im): re(re), im(im){}
    inline ~ComplexNumber() {}

    bool isOnAxis() const { return re.isZero() || im.isZero(); }
    bool isZero() const { return re.isZero() && im.isZero(); }

    void setRe(BigFloat re){ this->re = re; }
    void setIm(BigFloat im){ this->im = im; }

    BigFloat getRe(){ return re; }
    BigFloat getIm(){ return im; }

    QString toString() const;

private:

    BigFloat re;
    BigFloat im;
};

#endif // COMPLEXNUMBER_H
