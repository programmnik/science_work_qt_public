#include "sphereinteraction.h"
#include <GL/gl.h>
#include <cmath>
#include <QDebug>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// const float SPHERE_RADIUS = 0.5f;

// ============= КОНСТРУКТОР / ДЕСТРУКТОР =============

SphereInteraction::SphereInteraction(CameraCore* camera)
    : m_pointColor(255, 255, 0)      // Желтый (QColor)
    , m_projectionColor(255, 165, 0) // Оранжевый (QColor)
    , m_pointSize(6.0f)
    , m_showProjections(true)
    , m_showLabels(true)
    , m_cacheValid(false)
    , m_camera(camera)
{
}

SphereInteraction::~SphereInteraction()
{
    clearAllPoints();
}

// ============= УПРАВЛЕНИЕ ТОЧКАМИ =============

bool SphereInteraction::addPointFromScreen(int screenX, int screenY,
                                           int screenWidth, int screenHeight,
                                           float zoom)
{
    if (!m_camera) return false;

    // Получаем позицию камеры
    QVector3D rayOrigin = m_camera->getCameraPos();

    // Нормализованные координаты экрана (-1 до 1)
    // Как в Python: nx = (screen_x - screen_center[0]) / zoom
    //              ny = (screen_center[1] - screen_y) / zoom
    float nx = (screenX - screenWidth / 2.0f) / (zoom * 200.0f);
    float ny = (screenHeight / 2.0f - screenY) / (zoom * 200.0f);

    // Направление луча: front + right * nx + up * ny
    QVector3D rayDirection = m_camera->getCameraFront() +
                             m_camera->getCameraRight() * nx +
                             m_camera->getCameraUp() * ny;
    rayDirection.normalize();

    // Ищем пересечение со сферой
    QVector3D intersection = raySphereIntersection(rayOrigin, rayDirection);

    if (intersection.isNull()) {
        return false;
    }

    addPoint(intersection);
    return true;
}

void SphereInteraction::addPoint(const QVector3D& spherePos)
{
    float dist = spherePos.length();
    if (dist < 0.001f) return;

    QVector3D normalized = spherePos.normalized() * SPHERE_RADIUS;

    SpherePoint point(normalized);
    point.setLabel(QString("P%1").arg(m_points.size() + 1));
    point.setColor(m_pointColor);  // Устанавливаем цвет из настроек

    m_points.append(point);
    m_cacheValid = false;
}

void SphereInteraction::addPoint(float x, float y, float z)
{
    addPoint(QVector3D(x, y, z));
}

void SphereInteraction::removePoint(int index)
{
    if (index >= 0 && index < m_points.size()) {
        m_points.removeAt(index);
        m_cacheValid = false;
    }
}

void SphereInteraction::removeLastPoint()
{
    if (!m_points.isEmpty()) {
        m_points.removeLast();
        m_cacheValid = false;
    }
}

void SphereInteraction::clearAllPoints()
{
    m_points.clear();
    m_cacheValid = false;
}

const SpherePoint* SphereInteraction::getPoint(int index) const
{
    if (index >= 0 && index < m_points.size()) {
        return &m_points[index];
    }
    return nullptr;
}

// ============= ПРОЕКЦИИ =============

QVector3D SphereInteraction::getPlaneProjection(int index) const
{
    if (index >= 0 && index < m_points.size()) {
        return m_points[index].getPlaneProjection();
    }
    return QVector3D();
}

QVector2D SphereInteraction::getOrthogonalProjection(int index) const
{
    if (index >= 0 && index < m_points.size()) {
        return m_points[index].getOrthogonalProjection();
    }
    return QVector2D();
}

// ============= ОТРИСОВКА =============

void SphereInteraction::draw(CameraCore* camera, int screenWidth,
                             int screenHeight, float zoom)
{
    if (!camera) return;

    // Рисуем проекции (линии от северного полюса через точку)
    if (m_showProjections) {
        glColor3f(m_projectionColor.redF(),
                  m_projectionColor.greenF(),
                  m_projectionColor.blueF());
        glLineWidth(1.5f);

        glBegin(GL_LINES);
        for (const SpherePoint& point : m_points) {
            QVector3D spherePos = point.getSpherePosition();
            QVector3D planeProj = point.getPlaneProjection();

            // Северный полюс (0, 0, 1.0)
            QVector3D northPole(0.0f, 0.0f, 1.0f);

            glVertex3f(northPole.x(), northPole.y(), northPole.z());
            glVertex3f(spherePos.x(), spherePos.y(), spherePos.z());

            glVertex3f(spherePos.x(), spherePos.y(), spherePos.z());
            glVertex3f(planeProj.x(), planeProj.y(), 0.0f);
        }
        glEnd();
        glLineWidth(1.0f);
    }

    // Рисуем точки
    for (const SpherePoint& point : m_points) {
        drawPoint(point, camera, screenWidth, screenHeight, zoom);
        drawProjection(point, camera, screenWidth, screenHeight, zoom);
    }
}

void SphereInteraction::drawPoint(const SpherePoint& point, CameraCore* camera,
                                  int screenWidth, int screenHeight, float zoom)
{
    Q_UNUSED(camera);
    Q_UNUSED(screenWidth);
    Q_UNUSED(screenHeight);
    Q_UNUSED(zoom);

    QVector3D pos = point.getSpherePosition();
    QColor color = point.getColor();  // Теперь это QColor!

    // Рисуем точку на сфере
    glColor3f(color.redF(), color.greenF(), color.blueF());
    glPointSize(m_pointSize);
    glBegin(GL_POINTS);
    glVertex3f(pos.x(), pos.y(), pos.z());
    glEnd();
    glPointSize(1.0f);
}

void SphereInteraction::drawProjection(const SpherePoint& point, CameraCore* camera,
                                       int screenWidth, int screenHeight, float zoom)
{
    Q_UNUSED(camera);
    Q_UNUSED(screenWidth);
    Q_UNUSED(screenHeight);
    Q_UNUSED(zoom);

    QVector3D planeProj = point.getPlaneProjection();
    QVector3D spherePos = point.getSpherePosition();

    // Рисуем проекцию на плоскости (зеленым цветом)
    glColor3f(0.0f, 1.0f, 0.0f);
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    glVertex3f(planeProj.x(), planeProj.y(), 0.0f);
    glEnd();
    glPointSize(1.0f);

    // Рисуем линию от точки на сфере к проекции на плоскости
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex3f(spherePos.x(), spherePos.y(), spherePos.z());
    glVertex3f(planeProj.x(), planeProj.y(), 0.0f);
    glEnd();
    glLineWidth(1.0f);
}

// ============= ВНУТРЕННИЕ МЕТОДЫ =============

QVector3D SphereInteraction::raySphereIntersection(const QVector3D& rayOrigin,
                                                   const QVector3D& rayDirection) const
{
    // Центр сферы (0, 0, SPHERE_RADIUS)
    QVector3D sphereCenter(0.0f, 0.0f, SPHERE_RADIUS);
    float sphereRadius = SPHERE_RADIUS;

    // Вектор от начала луча к центру сферы
    QVector3D oc = rayOrigin - sphereCenter;

    // Коэффициенты квадратного уравнения
    float a = QVector3D::dotProduct(rayDirection, rayDirection);
    float b = 2.0f * QVector3D::dotProduct(oc, rayDirection);
    float c = QVector3D::dotProduct(oc, oc) - sphereRadius * sphereRadius;

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return QVector3D(); // Нет пересечения
    }

    float sqrtDisc = sqrt(discriminant);
    float t1 = (-b - sqrtDisc) / (2 * a);
    float t2 = (-b + sqrtDisc) / (2 * a);

    // Выбираем ближайшую положительную точку (перед камерой)
    float t = -1.0f;
    if (t1 >= 0 && t2 >= 0) {
        t = qMin(t1, t2);
    } else if (t1 >= 0) {
        t = t1;
    } else if (t2 >= 0) {
        t = t2;
    }

    if (t < 0) {
        return QVector3D(); // Точка позади камеры
    }

    QVector3D point = rayOrigin + rayDirection * t;

    // Проверяем, что точка на видимой стороне сферы
    QVector3D normal = (point - sphereCenter).normalized();
    QVector3D toCamera = (rayOrigin - point).normalized();

    // Если нормаль и вектор к камере направлены в одну сторону - точка видима
    if (QVector3D::dotProduct(normal, toCamera) < 0) {
        return QVector3D(); // Точка на обратной стороне
    }

    return point;
}
QVector3D SphereInteraction::unprojectScreenToRay(int screenX, int screenY,
                                                  int screenWidth, int screenHeight,
                                                  float zoom, CameraCore* camera) const
{
    if (!camera) return QVector3D();

    float nx = (screenX - screenWidth / 2.0f) / (zoom * 200.0f);
    float ny = (screenHeight / 2.0f - screenY) / (zoom * 200.0f);

    QVector3D rayOrigin = camera->getCameraPos();
    QVector3D rayDirection = camera->getCameraFront() +
                             camera->getCameraRight() * nx +
                             camera->getCameraUp() * ny;
    rayDirection.normalize();

    return rayDirection;
}

QVector2D SphereInteraction::project3DTo2D(const QVector3D& point,
                                           int screenWidth, int screenHeight,
                                           float zoom) const
{
    if (!m_camera) return QVector2D();

    QVector3D rotated = m_camera->applyRotationToPoint(point);

    float z = rotated.z();
    if (z <= 0.01f) return QVector2D();

    float scale = zoom * 200.0f / z;
    float x2d = screenWidth / 2.0f + rotated.x() * scale;
    float y2d = screenHeight / 2.0f - rotated.y() * scale;

    return QVector2D(x2d, y2d);
}

// ============= ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ =============

bool SphereInteraction::hasPoint(const QVector3D& pos, float tolerance) const
{
    for (const SpherePoint& point : m_points) {
        QVector3D diff = point.getSpherePosition() - pos;
        if (diff.length() < tolerance) {
            return true;
        }
    }
    return false;
}

int SphereInteraction::findNearestPoint(const QVector3D& pos, float maxDistance) const
{
    int nearest = -1;
    float minDist = maxDistance;

    for (int i = 0; i < m_points.size(); ++i) {
        QVector3D diff = m_points[i].getSpherePosition() - pos;
        float dist = diff.length();
        if (dist < minDist) {
            minDist = dist;
            nearest = i;
        }
    }

    return nearest;
}
