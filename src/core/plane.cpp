#include "plane.h"
#include <GL/gl.h>
#include "constants.h"

Plane::Plane()
    : viewDistance(100.0f)
    , stepX(1.0f)
    , stepY(1.0f)
{
}

void Plane::setViewDistance(float distance)
{
    viewDistance = distance;
}

void Plane::calculateVisibleRange(const Point &cameraPoint,
                                  float &minX, float &maxX,
                                  float &minY, float &maxY)
{
    float camX = cameraPoint.getX();
    float camY = cameraPoint.getY();

    minX = camX - viewDistance;
    maxX = camX + viewDistance;
    minY = camY - viewDistance;
    maxY = camY + viewDistance;
}

void Plane::draw(const Point& cameraPoint)
{
    float minX, maxX, minY, maxY;

    calculateVisibleRange(cameraPoint, minX, maxX, minY, maxY);

    float startX = std::floor(minX / stepX) * stepX;
    float startY = std::floor(minY / stepY) * stepY;


    glColor3f(Colors::PLANE_R,
              Colors::PLANE_G,
              Colors::PLANE_B);

    glBegin(GL_LINES);

    for (float x = startX; x <= maxX; x += stepX) {
        if(x == 0) continue;
        glVertex3f(x, minY, 0);
        glVertex3f(x, maxY, 0);
    }

    for (float y = startY; y <= maxY; y += stepY) {
        if(y == 0) continue;
        glVertex3f(minX, y, 0);
        glVertex3f(maxX, y, 0);
    }

    glEnd();

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    if(minY <= 0 && maxY >= 0){
        glColor3f(Colors::AXIS_X_R,
                  Colors::AXIS_X_G,
                  Colors::AXIS_X_B);
        glVertex3f(minX, 0, 0);
        glVertex3f(maxX, 0, 0);
    }

    if(minX <= 0 && maxX >= 0){
        glColor3f(Colors::AXIS_Y_R,
                  Colors::AXIS_Y_G,
                  Colors::AXIS_Y_B);
        glVertex3f(0, minY, 0);
        glVertex3f(0, maxY, 0);
    }
    glEnd();
    glLineWidth(1.0f);
}
