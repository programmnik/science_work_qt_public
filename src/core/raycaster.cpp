#include "raycaster.h"
#include <cmath>

Raycaster::Ray Raycaster::screenToRay(const QVector3D& cameraPos,
                                      const QVector3D& cameraFront,
                                      const QVector3D& cameraRight,
                                      const QVector3D& cameraUp,
                                      int screenX, int screenY,
                                      int screenWidth, int screenHeight,
                                      const FrustumParams& frustum)
{
    // 1. NDC координаты
    float ndcX = (2.0f * screenX / screenWidth) - 1.0f;
    float ndcY = 1.0f - (2.0f * screenY / screenHeight);

    // 2. Координаты на near plane
    float worldX = frustum.left + (frustum.right - frustum.left) * (ndcX + 1.0f) / 2.0f;
    float worldY = frustum.bottom + (frustum.top - frustum.bottom) * (ndcY + 1.0f) / 2.0f;

    // 3. Точка на near plane в мировых координатах
    QVector3D pointOnNearPlane = cameraPos
                                 + cameraFront * frustum.nearPlane
                                 + cameraRight * worldX
                                 + cameraUp * worldY;

    // 4. Создаем луч
    Ray result;
    result.origin = pointOnNearPlane;
    result.direction = (pointOnNearPlane - cameraPos).normalized();

    return result;
}

bool Raycaster::intersectsSphere(const QVector3D& rayOrigin,
                                 const QVector3D& rayDirection,
                                 const QVector3D& sphereCenter,
                                 float sphereRadius,
                                 QVector3D& hitPoint,
                                 float& hitDistance)
{
    // Вектор от начала луча к центру сферы
    QVector3D oc = rayOrigin - sphereCenter;

    // Коэффициенты квадратного уравнения
    float a = QVector3D::dotProduct(rayDirection, rayDirection);
    float b = 2.0f * QVector3D::dotProduct(oc, rayDirection);
    float c = QVector3D::dotProduct(oc, oc) - sphereRadius * sphereRadius;

    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f) {
        return false;
    }

    float sqrtDisc = sqrt(discriminant);
    float t1 = (-b - sqrtDisc) / (2.0f * a);
    float t2 = (-b + sqrtDisc) / (2.0f * a);

    // Выбираем ближайшее положительное t
    float t = -1.0f;
    if (t1 >= 0.0f && t2 >= 0.0f) {
        t = qMin(t1, t2);
    } else if (t1 >= 0.0f) {
        t = t1;
    } else if (t2 >= 0.0f) {
        t = t2;
    } else {
        return false;
    }

    hitPoint = rayOrigin + rayDirection * t;
    hitDistance = t;

    return true;
}
