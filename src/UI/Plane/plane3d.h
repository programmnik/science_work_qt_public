#ifndef PLANE3D_H
#define PLANE3D_H

#include "plane.h"

class Plane3d : public Plane
{
public:
    Plane3d();

    void draw(Camera* camera, QPainter* painter, int screenWidth, int screenHeight) override;

    void setViewDistance(float distance) { viewDistance = distance; }
    void setViewNumberDistance(float numDistance) { viewNumberDistance = numDistance; }

private:

    QVector<Label> generateLabels(Camera* camera, int screenWidth, int screenHeight) override;

    void drawGrid(const Point& cameraPoint);
    void drawAxes(const Point& cameraPoint);
    void calculateVisibleRange(const Point& cameraPoint,
                               float& minX, float& maxX,
                               float& minY, float& maxY,
                               float view);

    QPointF projectToScreen(const QVector3D& worldPos);

    bool isPointInFrontOfCamera(const QVector3D& point,
                                const QVector3D& cameraPos,
                                const QVector3D& cameraFront);

    float viewDistance;
    float viewNumberDistance;
};

#endif // PLANE3D_H
