#ifndef COMPLEXNUMBER_H
#define COMPLEXNUMBER_H

#include <QString>

class ComplexNumber
{
public:
    ComplexNumber();
    ComplexNumber(float re, float im);
    inline ~ComplexNumber() {}

    QString toString() const;
    bool isOnAxis() const;
    bool isZero() const;

    void setRe(float re){ this->re = re; }
    void setIm(float im){ this->im = im; }
    float getRe(){ return re; }
    float getIm(){ return im; }

private:

    float re;
    float im;
};

#endif // COMPLEXNUMBER_H
