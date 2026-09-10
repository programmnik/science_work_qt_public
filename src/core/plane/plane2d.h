#ifndef PLANE2D_H
#define PLANE2D_H

#include "plane.h"
#include "../camera/cameracore2d.h"

class Plane2d : public Plane
{
public:
    Plane2d();
    void draw(Camera* camera, QPainter* painter, int screenWidth, int screenHeight) override;

    void setLabelOffsetPercent(float percent) { m_labelOffsetPercent = percent; }

    float getLabelOffsetPercent() const { return m_labelOffsetPercent; }

private:
    QVector<Label> generateLabels(Camera* camera, int screenWidth, int screenHeight) override;
    void drawGrid(CameraCore2D* camera2d, int viewportWidth, int viewportHeight);

    float m_labelOffsetPercent = 0.1f;
};

#endif // PLANE2D_H
