#include "point.h"

Point::Point(float x, float y, float z,
             float rotX, float rotY, float rotZ)
    : x(x), y(y), z(z), rotX(rotX), rotY(rotY), rotZ(rotZ)
{
}

void Point::setPosition(float newX, float newY, float newZ){
    x = newX;
    y = newY;
    z = newZ;
}

void Point::move(float dx, float dy, float dz){
    x += dx;
    y += dy;
    z += dz;
}

void Point::rotate(float drotX, float drotY, float drotZ){
    rotX += drotX;
    rotY += drotY;
    rotZ += drotZ;
}
