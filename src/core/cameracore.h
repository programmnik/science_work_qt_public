#ifndef CAMERACORE_H
#define CAMERACORE_H

#include "point.h"
#include <QQuaternion>
#include <QVector3D>

class CameraCore
{
public:
    CameraCore();
    ~CameraCore();

    void setPoint(Point* point);
    void move(float forward, float right, float up);
    void rotate(float dx, float dy, float dz);  // dx - вверх/вниз, dy - влево/вправо
    void applyToOpenGL();

private:
    Point *point;

    // Кватернион для вращения
    QQuaternion orientation;
    QVector3D camera_pos;
    QVector3D camera_front;
    QVector3D camera_up;
    QVector3D camera_right;

    void updateCameraVectors();
};

#endif // CAMERACORE_H
