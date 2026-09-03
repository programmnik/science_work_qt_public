#ifndef PLANE_H
#define PLANE_H

#include "point.h"

class Plane
{
public:
    Plane();

    void draw(const Point& cameraPoint);
    void setViewDistance(float distance);

private:
    float viewDistance;
    float stepX;
    float stepY;

    void calculateVisibleRange(const Point& cameraPoint,
                               float& minX, float& maxX,
                               float& minY, float& maxY);
};

#endif // PLANE_H
