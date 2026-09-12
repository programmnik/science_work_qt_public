#ifndef SPHERE_H
#define SPHERE_H

#include "../Point/point.h"
#include "spherepoint.h"
#include <QVector3D>
#include <QVector>
#include "../Camera/cameracore3d.h"

class Sphere
{
public:
    Sphere();

    // Основные методы
    void draw(const Point& cameraPoint);
    void setRadius(float r);
    void setPosition(float x, float y, float z);
    void setColor(float r, float g, float b);

    // Геттеры
    QVector3D getCenter() const {
        return QVector3D(centerPoint.getX(), centerPoint.getY(), centerPoint.getZ());
    }
    QVector3D getNorthPole() const {
        QVector3D center = getCenter();
        return QVector3D(center.x(), center.y(), center.z() + radius);
    }
    float getRadius() const { return radius; }
    Point& getCenterPoint() { return centerPoint; }
    const Point& getCenterPoint() const { return centerPoint; }

    // Raycasting
    bool intersectsRay(const QVector3D& rayOrigin,
                       const QVector3D& rayDirection,
                       QVector3D& hitPoint) const;

    // ========== УПРАВЛЕНИЕ ТОЧКАМИ НА СФЕРЕ ==========
    void addPoint(const QVector3D& worldPoint);
    void addPoint(float x, float y, float z);
    void clearPoints();
    void removePoint(int index);
    void removeLastPoint();

    int getPointCount() const { return m_points.size(); }
    const SpherePoint* getPoint(int index) const;
    const QVector<SpherePoint>& getPoints() const { return m_points; }
    bool hasPoint(const QVector3D& pos, float tolerance = 0.01f) const;

    // ========== ОТРИСОВКА ТОЧЕК ==========
    void drawPoints(CameraCore3D* camera, int screenWidth, int screenHeight, float zoom);
    void setPointColor(const QColor& color) { m_pointColor = color; }
    void setPointSize(float size) { m_pointSize = size; }
    void setShowProjections(bool show) { m_showProjections = show; }
    void setProjectionColor(const QColor& color) { m_projectionColor = color; }

private:
    void drawWireframe();
    void drawProjection(const SpherePoint& point, CameraCore3D* camera,
                        int screenWidth, int screenHeight, float zoom);

    Point centerPoint;
    float radius;
    int rings;
    int sectors;
    float colorR, colorG, colorB;

    // Точки на сфере в МИРОВЫХ координатах
    QVector<SpherePoint> m_points;

    // Настройки отрисовки точек
    QColor m_pointColor = QColor(255, 255, 0);
    QColor m_projectionColor = QColor(255, 165, 0);
    float m_pointSize = 8.0f;
    bool m_showProjections = true;
};

#endif // SPHERE_H
