#ifndef SPHERE_H
#define SPHERE_H

#include "point.h"

class Sphere
{
public:
    Sphere();

    void draw(const Point& cameraPoint);
    void setRadius(float r);
    void setPosition(float x, float y, float z);
    void setColor(float r, float g, float b);

private:
    Point centerPoint;
    float radius;
    int rings;
    int sectors;
    float colorR, colorG, colorB;

    void drawWireframe();
};

#endif // SPHERE_H
