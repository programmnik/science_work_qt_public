#include "cameracore.h"
#include <GL/gl.h>
#include <cmath>

CameraCore::CameraCore(): point(nullptr) {}

CameraCore::~CameraCore(){}

void CameraCore::updateRotationMatrix(){
    if (!point) return;

    rotationMatrix.setToIdentity();
    rotationMatrix.rotate(point->getRotX() * 180.0f / M_PI, -1.0f, 0.0f, 0.0f);
    rotationMatrix.rotate(point->getRotY() * 180.0f / M_PI, 0.0f, 0.0f, 1.0f);
    rotationMatrix.rotate(point->getRotZ() * 180.0f / M_PI, 0.0f, 1.0f, 0.0f);
}

void CameraCore::setPoint(Point* newPoint){
    point = newPoint;
    updateRotationMatrix();
}

void CameraCore::move(float forward, float right, float up){
    if(!point) return;
    QVector3D localVector(right, up, forward);
    QVector3D worldVector = rotationMatrix * localVector;
    point->move(worldVector.x(), worldVector.y(), worldVector.z());
}

void CameraCore::rotate(float dx, float dy, float dz){
    if(!point) return;
    point->rotate(dx, dy, dz);
    updateRotationMatrix();
}

void CameraCore::applyToOpenGL(){
    if(!point) return;

    glRotatef(point->getRotX(), 1.0f, 0.0f, 0.0f);
    glRotatef(point->getRotY(), 0.0f, 1.0f, 0.0f);
    glRotatef(point->getRotZ(), 0.0f, 0.0f, 1.0f);
    glTranslatef(-point->getX(), -point->getY(), -point->getZ());
}
