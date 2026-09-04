#include "spherepoint.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const float SPHERE_RADIUS = 0.5f;

SpherePoint::SpherePoint()
    : m_spherePos(0, 0, 0)
    , m_color(255, 255, 0)  // Желтый (QColor)
    , m_label("P")
    , m_hasPlaneProjection(false)
{
}

SpherePoint::SpherePoint(const QVector3D& spherePos)
    : m_spherePos(spherePos)
    , m_color(255, 255, 0)  // Желтый (QColor)
    , m_label("P")
    , m_hasPlaneProjection(false)
{
}

SpherePoint::SpherePoint(float x, float y, float z)
    : m_spherePos(x, y, z)
    , m_color(255, 255, 0)  // Желтый (QColor)
    , m_label("P")
    , m_hasPlaneProjection(false)
{
}

QVector3D SpherePoint::getPlaneProjection() const
{
    // Стереографическая проекция для системы с центром в (0,0,R)
    float x = m_spherePos.x();
    float y = m_spherePos.y();
    float z = m_spherePos.z();

    // Проверка на северный полюс
    if (abs(2 * SPHERE_RADIUS - z) < 1e-10) {
        return QVector3D(0, 0, 0);
    }

    // Смещаем относительно центра сферы
    float z_centered = z - SPHERE_RADIUS;
    float denominator = SPHERE_RADIUS - z_centered;

    if (abs(denominator) < 1e-10) {
        return QVector3D(0, 0, 0);
    }

    float planeX = x / denominator;
    float planeY = y / denominator;

    return QVector3D(planeX, planeY, 0.0f);
}

void SpherePoint::setPlaneProjection(const QVector3D& planePos)
{
    float x = planePos.x();
    float y = planePos.y();
    float r_squared = x*x + y*y;
    float denominator = r_squared + 1.0f;

    float sphereX = x / denominator;
    float sphereY = y / denominator;
    float sphereZ = SPHERE_RADIUS * (r_squared - 1.0f) / denominator + SPHERE_RADIUS;

    m_spherePos = QVector3D(sphereX, sphereY, sphereZ);
    m_hasPlaneProjection = true;
}
