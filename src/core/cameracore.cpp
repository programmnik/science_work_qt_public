#include "cameracore.h"
#include <GL/gl.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

CameraCore::CameraCore() : point(nullptr)
{
    camera_pos = QVector3D(0.45f, -0.45f, 1.0f);
    orientation = QQuaternion();  // Identity

    // Начальные векторы
    camera_front = QVector3D(0.0f, 0.0f, -1.0f);
    camera_up = QVector3D(0.0f, 1.0f, 0.0f);
    camera_right = QVector3D(1.0f, 0.0f, 0.0f);
}

CameraCore::~CameraCore() {}

void CameraCore::updateCameraVectors()
{
    // Вращаем начальные векторы кватернионом
    camera_front = orientation.rotatedVector(QVector3D(0.0f, 0.0f, -1.0f));
    camera_up = orientation.rotatedVector(QVector3D(0.0f, 1.0f, 0.0f));
    camera_right = orientation.rotatedVector(QVector3D(1.0f, 0.0f, 0.0f));
}

void CameraCore::setPoint(Point* newPoint)
{
    point = newPoint;

    // ВАЖНО: Обновляем ориентацию из углов точки!
    if (point) {
        // Получаем углы из Point (в градусах)
        float rotX = point->getRotX();
        float rotY = point->getRotY();
        float rotZ = point->getRotZ();

        // Создаем кватернионы для каждого угла
        QQuaternion qPitch = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, rotX);
        QQuaternion qYaw = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, rotY);
        QQuaternion qRoll = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, rotZ);

        // Локальное вращение: умножаем СПРАВА
        orientation = qYaw * qPitch * qRoll;
        orientation.normalize();

        updateCameraVectors();
    }
}

void CameraCore::move(float forward, float right, float up)
{
    if (!point) return;

    QVector3D moveVector = camera_front * forward + camera_right * right + camera_up * up;
    point->move(moveVector.x(), moveVector.y(), moveVector.z());
}

void CameraCore::rotate(float dx, float dy, float dz)
{
    if (!point) return;

    // ЛОКАЛЬНОЕ вращение: умножаем СПРАВА!
    QQuaternion qPitch = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, dx);
    QQuaternion qYaw = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, dy);
    QQuaternion qRoll = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, dz);

    // Умножаем СПРАВА для локального вращения
    orientation = orientation * qYaw * qPitch * qRoll;
    orientation.normalize();

    updateCameraVectors();

    // Обновляем углы в Point
    point->rotate(dx, dy, dz);
}

void CameraCore::applyToOpenGL()
{
    if (!point) return;

    QVector3D pos(point->getX(), point->getY(), point->getZ());

    // Матрица вида
    float viewMatrix[16] = {
        camera_right.x(), camera_up.x(), -camera_front.x(), 0.0f,
        camera_right.y(), camera_up.y(), -camera_front.y(), 0.0f,
        camera_right.z(), camera_up.z(), -camera_front.z(), 0.0f,
        -QVector3D::dotProduct(camera_right, pos),
        -QVector3D::dotProduct(camera_up, pos),
        QVector3D::dotProduct(camera_front, pos),
        1.0f
    };

    glLoadMatrixf(viewMatrix);
}
