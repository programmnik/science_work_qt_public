#ifndef CAMERA_H
#define CAMERA_H

#include "../point.h"
#include <QVector3D>
#include <QQuaternion>
#include "../fpscontroller.h"

class Camera
{
public:
    Camera();
    virtual ~Camera() = default;

    // Чисто виртуальные методы (должны быть реализованы в наследниках)
    virtual void applyToOpenGL() = 0;
    virtual void move(float dx, float dy, float dz) = 0;

    virtual void rotate(float pitch, float yaw, float roll);
    virtual bool processEvent(uint scanCode, FPSController& fpsController);
    virtual void zoom(float factor);

    // Общие геттеры
    virtual QVector3D getCameraPos() const { return camera_pos; }
    virtual QVector3D getCameraFront() const { return camera_front; }
    virtual QVector3D getCameraUp() const { return camera_up; }
    virtual QVector3D getCameraRight() const { return camera_right; }
    QVector3D getPointPosition() const {
        return point ? QVector3D(point->getX(), point->getY(), point->getZ()) : QVector3D();
    }
    Point* getPoint(){ return point; }
    float getZoom(){ return m_zoom; }

    void setMoveSpeed(float speed){ moveSpeed = speed; }
    void setRotSpeed(float speed){ rotSpeed = speed; }
    void setPoint(Point* point);
    void setZoom(float zoom){ m_zoom = zoom; }
    void setZoomRange(float min, float max){ min_zoom = min; max_zoom = max; }

protected:
    Point *point;

    // Общие методы для наследников
    void updateCameraVectors();

    // Общие поля
    QQuaternion orientation;
    QVector3D camera_pos;
    QVector3D camera_front;
    QVector3D camera_up;
    QVector3D camera_right;
    float moveSpeed;
    float rotSpeed;

    float m_zoom;
    float min_zoom;
    float max_zoom;
};

#endif // CAMERA_H
