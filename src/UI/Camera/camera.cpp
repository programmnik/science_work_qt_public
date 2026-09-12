#include "camera.h"
#include <cmath>

Camera::Camera()
    : point(nullptr)
    , moveSpeed(1.0f)
    , rotSpeed(1.0f)
    , m_zoom(1.0f)
    , min_zoom(1.0f)
    , max_zoom(1.0f)
{
    camera_pos = QVector3D(0.45f, -0.45f, 1.0f);
    orientation = QQuaternion();  // Identity

    // Начальные векторы
    camera_front = QVector3D(0.0f, 0.0f, -1.0f);
    camera_up = QVector3D(0.0f, 1.0f, 0.0f);
    camera_right = QVector3D(1.0f, 0.0f, 0.0f);
}

void Camera::rotate(float pitch, float yaw, float roll)
{
    Q_UNUSED(pitch);
    Q_UNUSED(yaw);
    Q_UNUSED(roll);
    // камера не вращается
}

void Camera::updateCameraVectors()
{
    // Вращаем начальные векторы кватернионом
    camera_pos = QVector3D(point->getX(), point->getY(), point->getZ());
    camera_front = orientation.rotatedVector(QVector3D(0.0f, 0.0f, -1.0f));
    camera_up = orientation.rotatedVector(QVector3D(0.0f, 1.0f, 0.0f));
    camera_right = orientation.rotatedVector(QVector3D(1.0f, 0.0f, 0.0f));
}

bool Camera::processEvent(uint scanCode, FPSController& fpsController){
    switch (scanCode) {
        case 25: fpsController.addMovement(moveSpeed, 0, 0); break; //W
        case 39: fpsController.addMovement(-moveSpeed, 0, 0); break;//S
        case 38: fpsController.addMovement(0, -moveSpeed, 0); break;//A
        case 40: fpsController.addMovement(0, moveSpeed, 0); break; //D
        default: return false;
    }
    return true;
}

void Camera::zoom(float factor)
{
    m_zoom *= factor;
    if (m_zoom < min_zoom) m_zoom = min_zoom;
    if (m_zoom > max_zoom) m_zoom = max_zoom;
}

void Camera::setPoint(Point* newPoint)
{
    point = newPoint;

    // Обновляем ориентацию из углов точки
    if (point) {
        // Получаем углы из Point
        float rotX = point->getRotX();
        float rotY = point->getRotY();
        float rotZ = point->getRotZ();

        // Создаем кватернионы для каждого угла
        QQuaternion qPitch = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, rotX);
        QQuaternion qYaw = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, rotY);
        QQuaternion qRoll = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, rotZ);

        // Для локального вращения умножаем справа, для глобального с
        orientation = qYaw * qPitch * qRoll;
        orientation.normalize();

        updateCameraVectors();
    }
}
