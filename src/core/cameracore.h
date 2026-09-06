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

    enum RotationMode {
        ModeLocal = 0,      // Локальное вращение
        ModeFixedYaw = 1    // Фиксированная ось Y для yaw
    };

    void setRotationMode(RotationMode mode) { m_rotationMode = mode; }
    void changeRotationMode() { m_rotationMode = m_rotationMode == ModeLocal ? ModeFixedYaw : ModeLocal; }
    RotationMode getRotationMode() const { return m_rotationMode; }

    // Геттеры для векторов камеры (добавлены)
    QVector3D getCameraPos() const { return camera_pos; }
    QVector3D getCameraFront() const { return camera_front; }
    QVector3D getCameraUp() const { return camera_up; }
    QVector3D getCameraRight() const { return camera_right; }

    // Геттер для позиции точки (если нужно)
    QVector3D getPointPosition() const {
        return point ? QVector3D(point->getX(), point->getY(), point->getZ()) : QVector3D();
    }

private:
    Point *point;
    RotationMode m_rotationMode = ModeFixedYaw;

    // Кватернион для вращения
    QQuaternion orientation;
    QVector3D camera_pos;
    QVector3D camera_front;
    QVector3D camera_up;
    QVector3D camera_right;

    void updateCameraVectors();
    void rotateLocal(float dx, float dy, float dz);
    void rotateFixedYaw(float dx, float dy, float dz);
};

#endif // CAMERACORE_H
