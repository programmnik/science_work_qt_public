#ifndef CAMERACORE_H
#define CAMERACORE_H

#include "point.h"
#include <QMatrix4x4>

class CameraCore
{
public:
    CameraCore();
    ~CameraCore();

    void setPoint(Point* point);
    void move(float forward, float right, float up);
    void rotate(float dx, float dy, float dz);
    void applyToOpenGL();

private:
    Point *point;
    QMatrix4x4 rotationMatrix;
    void updateRotationMatrix();
};

#endif // CAMERACORE_H
