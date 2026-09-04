#ifndef SPHEREINTERACTION_H
#define SPHEREINTERACTION_H

#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QColor>
#include "spherepoint.h"
#include "cameracore.h"

const float SPHERE_RADIUS = 0.5f;

class SphereInteraction
{
public:
    SphereInteraction(CameraCore* camera);
    ~SphereInteraction();

    // ========== УПРАВЛЕНИЕ ТОЧКАМИ ==========
    bool addPointFromScreen(int screenX, int screenY,
                            int screenWidth, int screenHeight,
                            float zoom);
    void addPoint(const QVector3D& spherePos);
    void addPoint(float x, float y, float z);
    void removePoint(int index);
    void removeLastPoint();
    void clearAllPoints();

    int getPointCount() const { return m_points.size(); }
    const SpherePoint* getPoint(int index) const;
    QVector<SpherePoint>& getPoints() { return m_points; }
    const QVector<SpherePoint>& getPoints() const { return m_points; }

    // ========== ПРОЕКЦИИ ==========
    QVector3D getPlaneProjection(int index) const;
    QVector2D getOrthogonalProjection(int index) const;

    // ========== ОТРИСОВКА ==========
    void draw(CameraCore* camera, int screenWidth, int screenHeight, float zoom);

    // Настройки отрисовки (используем QColor)
    void setPointColor(const QColor& color) { m_pointColor = color; }
    void setProjectionColor(const QColor& color) { m_projectionColor = color; }
    void setPointSize(float size) { m_pointSize = size; }
    void setShowProjections(bool show) { m_showProjections = show; }
    void setShowLabels(bool show) { m_showLabels = show; }

    // ========== ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ==========
    bool hasPoint(const QVector3D& pos, float tolerance = 0.01f) const;
    int findNearestPoint(const QVector3D& pos, float maxDistance = 0.1f) const;

private:
    // ========== ВНУТРЕННИЕ МЕТОДЫ ==========
    QVector3D raySphereIntersection(const QVector3D& rayOrigin,
                                    const QVector3D& rayDirection) const;
    QVector3D unprojectScreenToRay(int screenX, int screenY,
                                   int screenWidth, int screenHeight,
                                   float zoom, CameraCore* camera) const;
    QVector2D project3DTo2D(const QVector3D& point,
                            int screenWidth, int screenHeight,
                            float zoom) const;
    void drawPoint(const SpherePoint& point, CameraCore* camera,
                   int screenWidth, int screenHeight, float zoom);
    void drawProjection(const SpherePoint& point, CameraCore* camera,
                        int screenWidth, int screenHeight, float zoom);

    // ========== ДАННЫЕ ==========
    QVector<SpherePoint> m_points;

    // Настройки отрисовки (QColor)
    QColor m_pointColor;
    QColor m_projectionColor;
    float m_pointSize;
    bool m_showProjections;
    bool m_showLabels;

    // Кэш для оптимизации
    mutable QVector<QVector3D> m_cachedProjections;
    mutable bool m_cacheValid;

    CameraCore* m_camera;
};

#endif // SPHEREINTERACTION_H
