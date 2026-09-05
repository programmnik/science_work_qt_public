#include "sphere.h"
#include "constants.h"
#include <GL/gl.h>
#include <cmath>
#include <QMatrix4x4>
#include <QDebug>

Sphere::Sphere()
    : centerPoint(0, 0, 0, 90, 0, 0)
    , radius(SphereConfig::RADIUS)
    , rings(SphereConfig::RINGS_COUNT)
    , sectors(SphereConfig::SECTORS_COUNT)
    , colorR(Colors::SPHERE_R)
    , colorG(Colors::SPHERE_G)
    , colorB(Colors::SPHERE_B)
{
}

// ==================== ОСНОВНЫЕ МЕТОДЫ ====================

void Sphere::setRadius(float r)
{
    radius = r;
}

void Sphere::setPosition(float x, float y, float z)
{
    centerPoint.setPosition(x, y, z);
}

void Sphere::setColor(float r, float g, float b)
{
    colorR = r;
    colorG = g;
    colorB = b;
}

// ==================== RAYCASTING ====================

bool Sphere::intersectsRay(const QVector3D& rayOrigin,
                           const QVector3D& rayDirection,
                           QVector3D& hitPoint) const
{
    QVector3D sphereCenter = getCenter();
    float sphereRadius = getRadius();

    QVector3D oc = rayOrigin - sphereCenter;
    float a = QVector3D::dotProduct(rayDirection, rayDirection);
    float b = 2.0f * QVector3D::dotProduct(oc, rayDirection);
    float c = QVector3D::dotProduct(oc, oc) - sphereRadius * sphereRadius;

    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f) return false;

    float sqrtDisc = sqrt(discriminant);
    float t1 = (-b - sqrtDisc) / (2.0f * a);
    float t2 = (-b + sqrtDisc) / (2.0f * a);

    float t = -1.0f;
    if (t1 >= 0.0f && t2 >= 0.0f) t = qMin(t1, t2);
    else if (t1 >= 0.0f) t = t1;
    else if (t2 >= 0.0f) t = t2;

    if (t < 0.0f) return false;

    hitPoint = rayOrigin + rayDirection * t;

    QVector3D normal = (hitPoint - sphereCenter).normalized();
    QVector3D toCamera = (rayOrigin - hitPoint).normalized();

    if (QVector3D::dotProduct(normal, toCamera) < 0.0f) {
        return false;  // Точка на обратной стороне
    }

    return true;
}

// ==================== УПРАВЛЕНИЕ ТОЧКАМИ ====================

void Sphere::addPoint(const QVector3D& worldPoint)
{
    QVector3D sphereCenter = getCenter();

    // Проверяем, что точка на сфере
    float dist = (worldPoint - sphereCenter).length();
    if (dist < 0.001f) return;
    if (dist < radius - 0.01f || dist > radius + 0.01f) {
        qDebug() << "Точка не на сфере! Расстояние:" << dist << "(должно быть" << radius << ")";
        return;
    }

    // Проверяем, нет ли уже такой точки
    if (hasPoint(worldPoint, 0.05f)) {
        qDebug() << "Точка уже существует";
        return;
    }

    // Сохраняем точку в МИРОВЫХ координатах
    SpherePoint point(worldPoint);
    point.setLabel(QString("P%1").arg(m_points.size() + 1));
    point.setColor(m_pointColor);

    m_points.append(point);
    qDebug() << "Точка добавлена в мировых координатах:" << worldPoint;
}

void Sphere::addPoint(float x, float y, float z)
{
    addPoint(QVector3D(x, y, z));
}

void Sphere::clearPoints()
{
    m_points.clear();
}

void Sphere::removePoint(int index)
{
    if (index >= 0 && index < m_points.size()) {
        m_points.removeAt(index);
    }
}

void Sphere::removeLastPoint()
{
    if (!m_points.isEmpty()) {
        m_points.removeLast();
    }
}

const SpherePoint* Sphere::getPoint(int index) const
{
    if (index >= 0 && index < m_points.size()) {
        return &m_points[index];
    }
    return nullptr;
}

bool Sphere::hasPoint(const QVector3D& pos, float tolerance) const
{
    for (const SpherePoint& point : m_points) {
        QVector3D diff = point.getSpherePosition() - pos;
        if (diff.length() < tolerance) {
            return true;
        }
    }
    return false;
}

// ==================== ОТРИСОВКА ====================

void Sphere::drawWireframe()
{
    glColor3f(colorR, colorG, colorB);

    QMatrix4x4 rotationMatrix;
    rotationMatrix.rotate(centerPoint.getRotX(), 1, 0, 0);
    rotationMatrix.rotate(centerPoint.getRotY(), 0, 1, 0);
    rotationMatrix.rotate(centerPoint.getRotZ(), 0, 0, 1);

    for (int i = 0; i <= rings; i++){
        float theta = M_PI * i / rings;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);
        float y = radius * cosTheta;
        float r = radius * sinTheta;

        glBegin(GL_LINE_LOOP);
        for(int j = 0; j <= sectors; j++){
            float phi = 2 * M_PI * j / sectors;
            float x = r * cos(phi);
            float z = r * sin(phi);

            QVector4D point(x, y, z, 1.0f);
            QVector4D rotatedPoint = rotationMatrix * point;

            glVertex3f(rotatedPoint.x(), rotatedPoint.y(), rotatedPoint.z());
        }
        glEnd();
    }

    for (int j = 0; j <= sectors; j++){
        float phi = 2 * M_PI * j / sectors;
        float cosPhi = cos(phi);
        float sinPhi = sin(phi);

        glBegin(GL_LINE_LOOP);
        for(int i = 0; i <= rings; i++){
            float theta = M_PI * i / rings;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            float x = radius * sinTheta * cosPhi;
            float y = radius * cosTheta;
            float z = radius * sinTheta * sinPhi;

            QVector4D point(x, y, z, 1.0f);
            QVector4D rotatedPoint = rotationMatrix * point;

            glVertex3f(rotatedPoint.x(), rotatedPoint.y(), rotatedPoint.z());
        }
        glEnd();
    }
}

void Sphere::draw(const Point& cameraPoint)
{
    Q_UNUSED(cameraPoint);
    glPushMatrix();
    glTranslatef(centerPoint.getX(), centerPoint.getY(), centerPoint.getZ());
    drawWireframe();
    glPopMatrix();
}

// ==================== ОТРИСОВКА ТОЧЕК ====================

void Sphere::drawPoints(CameraCore* camera, int screenWidth, int screenHeight, float zoom)
{
    Q_UNUSED(camera);
    Q_UNUSED(screenWidth);
    Q_UNUSED(screenHeight);
    Q_UNUSED(zoom);

    if (m_points.isEmpty()) return;

    // Северный полюс вычисляем из параметров сферы
    QVector3D northPole = getNorthPole();

    if (m_showProjections) {
        glColor3f(m_projectionColor.redF(),
                  m_projectionColor.greenF(),
                  m_projectionColor.blueF());
        glLineWidth(1.5f);

        glBegin(GL_LINES);
        for (const SpherePoint& point : m_points) {
            QVector3D worldPos = point.getSpherePosition();
            QVector3D worldProj = point.getPlaneProjection(northPole);

            // Отрезок: северный полюс → проекция на плоскость
            glVertex3f(northPole.x(), northPole.y(), northPole.z());
            glVertex3f(worldProj.x(), worldProj.y(), worldProj.z());
        }
        glEnd();
        glLineWidth(1.0f);
    }

    // Рисуем точки на сфере
    glColor3f(m_pointColor.redF(), m_pointColor.greenF(), m_pointColor.blueF());
    glPointSize(m_pointSize);
    glBegin(GL_POINTS);
    for (const SpherePoint& point : m_points) {
        QVector3D pos = point.getSpherePosition();
        glVertex3f(pos.x(), pos.y(), pos.z());
    }
    glEnd();
    glPointSize(1.0f);

    // Рисуем проекции на плоскость
    glColor3f(0.0f, 1.0f, 0.0f);
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    for (const SpherePoint& point : m_points) {
        QVector3D worldProj = point.getPlaneProjection(northPole);
        glVertex3f(worldProj.x(), worldProj.y(), worldProj.z());
    }
    glEnd();
    glPointSize(1.0f);
}

void Sphere::drawProjection(const SpherePoint& point, CameraCore* camera,
                            int screenWidth, int screenHeight, float zoom)
{
    Q_UNUSED(camera);
    Q_UNUSED(screenWidth);
    Q_UNUSED(screenHeight);
    Q_UNUSED(zoom);

    QVector3D northPole = getNorthPole();

    QVector3D spherePos = point.getSpherePosition();
    QVector3D planeProj = point.getPlaneProjection(northPole);

    // Рисуем линию от точки на сфере к проекции на плоскость
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex3f(spherePos.x(), spherePos.y(), spherePos.z());
    glVertex3f(planeProj.x(), planeProj.y(), planeProj.z());
    glEnd();
    glLineWidth(1.0f);
}
