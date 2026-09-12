#include "cameracore3d.h"
#include <GL/gl.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void CameraCore3D::move(float forward, float right, float up)
{
    if (!point) return;

    QVector3D moveVector = camera_front * forward + camera_right * right + camera_up * up;
    point->move(moveVector.x(), moveVector.y(), moveVector.z());
}

bool CameraCore3D::processEvent(uint scanCode, FPSController& fpsController){
    if(Camera::processEvent(scanCode, fpsController)) return true;

    switch (scanCode) {
        case 113: fpsController.addRotation(0, -rotSpeed, 0); break; // Left
        case 114: fpsController.addRotation(0, rotSpeed, 0); break;  // Right
        case 111: fpsController.addRotation(-rotSpeed, 0, 0); break; // Up
        case 116: fpsController.addRotation(rotSpeed, 0, 0); break;  // Down
        case 24:  fpsController.addMovement(0, 0, moveSpeed); break; // Q
        case 52:  fpsController.addMovement(0, 0, -moveSpeed); break;// Z
        default: return false;
    }

    return true;
}

// РЕЖИМЫ ВРАЩЕНИЯ

void CameraCore3D::rotateLocal(float dx, float dy, float dz)
{
    if (!point) return;

    // Создаем кватернионы для поворота вокруг локальных осей
    QQuaternion qPitch = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, dx);  // вокруг X (вверх/вниз)
    QQuaternion qYaw = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, dy);    // вокруг Y (влево/вправо)
    // QQuaternion qRoll = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, dz);   // вокруг Z (наклон) — ПОКА НЕ ИСПОЛЬЗУЕМ

    // Умножаем СПРАВА для локального вращения
    // orientation = orientation * qYaw * qPitch * qRoll;
    orientation = orientation * qYaw * qPitch;
    orientation.normalize();

    // Обновляем векторы камеры
    updateCameraVectors();

    // Сохраняем углы в Point (dz пока не используем)
    // point->rotate(dx, dy, dz);
    point->rotate(dx, dy, 0.0f);
}

void CameraCore3D::rotateFixedYaw(float dx, float dy, float dz)
{
    if (!point) return;

    QQuaternion qPitch = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, dx);
    QQuaternion qYaw = QQuaternion::fromAxisAndAngle(0.0f, 0.0f, 1.0f, dy);
    QQuaternion qRoll = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, dz);

    // Умножаем СЛЕВА для глобального вращения
    orientation = qYaw * qRoll * orientation * qPitch;
    orientation.normalize();

    updateCameraVectors();
    point->rotate(dx, dy, dz);
}

void CameraCore3D::rotate(float dx, float dy, float dz)
{
    if (!point) return;

    if (m_rotationMode == ModeLocal) {
        rotateLocal(dx, dy, dz);
    } else if (m_rotationMode == ModeFixedYaw) {
        rotateFixedYaw(dx, dy, dz);
    }
}

void CameraCore3D::applyToOpenGL()
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
