#include "spherepoint.h"
#include <cmath>

SpherePoint::SpherePoint()
    : m_spherePos(0, 0, 0)
    , m_color(255, 255, 0)
    , m_label("P")
    , m_hasPlaneProjection(false)
{
}

SpherePoint::SpherePoint(const QVector3D& spherePos)
    : m_spherePos(spherePos)
    , m_color(255, 255, 0)
    , m_label("P")
    , m_hasPlaneProjection(false)
{
}

SpherePoint::SpherePoint(float x, float y, float z)
    : m_spherePos(x, y, z)
    , m_color(255, 255, 0)
    , m_label("P")
    , m_hasPlaneProjection(false)
{
}

QVector3D SpherePoint::getPlaneProjection(const QVector3D& northPole) const
{
    // Уравнение прямой: P(t) = A + t * (B - A)
    // A = northPole, B = m_spherePos (в мировых координатах)

    QVector3D A = northPole;
    QVector3D B = m_spherePos;
    QVector3D direction = B - A;

    if (abs(direction.z()) < 1e-10) {
        return QVector3D(0, 0, 0);
    }

    // Находим t при z = 0
    float t = -A.z() / direction.z();
    QVector3D pointOnPlane = A + t * direction;

    return pointOnPlane;
}
