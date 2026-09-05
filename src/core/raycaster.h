#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <QVector3D>
#include <QDebug>

class Raycaster
{
public:
    // Структура для параметров frustum
    struct FrustumParams {
        float left, right, bottom, top;
        float nearPlane, farPlane;
        float fov;
        float aspect;
    };

    // Структура для луча
    struct Ray {
        QVector3D origin;      // Начало луча (точка на near plane)
        QVector3D direction;   // Направление луча (нормализованное)
    };

    // Создает луч из экранных координат
    static Ray screenToRay(const QVector3D& cameraPos,
                           const QVector3D& cameraFront,
                           const QVector3D& cameraRight,
                           const QVector3D& cameraUp,
                           int screenX, int screenY,
                           int screenWidth, int screenHeight,
                           const FrustumParams& frustum);

    // Проверяет пересечение луча со сферой
    static bool intersectsSphere(const QVector3D& rayOrigin,
                                 const QVector3D& rayDirection,
                                 const QVector3D& sphereCenter,
                                 float sphereRadius,
                                 QVector3D& hitPoint,
                                 float& hitDistance);
};

#endif // RAYCASTER_H
