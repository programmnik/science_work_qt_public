#include "sphere.h"
#include "constants.h"
#include <GL/gl.h>
#include <cmath>
#include <QMatrix4x4>

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
    glPushMatrix();
    glTranslatef(centerPoint.getX(), centerPoint.getY(), centerPoint.getZ());
    drawWireframe();
    glPopMatrix();
}
