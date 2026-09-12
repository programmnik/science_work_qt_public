#include "cameracore2d.h"
#include <GL/gl.h>
#include <cmath>

CameraCore2D::CameraCore2D()
    : Camera()
{
    camera_pos = QVector3D(0.0f, 0.0f, 0.0f);
    updateProjection();
    this->setZoomRange(0.1f, 10.0f);
}

void CameraCore2D::move(float dx, float dy, float dz)
{
    Q_UNUSED(dz);
    camera_pos.setX(camera_pos.x() + dx);
    camera_pos.setY(camera_pos.y() + dy);
    updateProjection();
}

bool CameraCore2D::processEvent(uint scanCode, FPSController& fpsController){
    if(Camera::processEvent(scanCode, fpsController)) return true;

    switch (scanCode) {
        case 24: this->zoom(1.1f); break;
        case 52: this->zoom(-1.1f); break;
        default: return false;
    }

    return true;
}

void CameraCore2D::zoom(float factor)
{
    Camera::zoom(factor);
    updateProjection();
}

void CameraCore2D::updateProjection()
{
    float halfSize = m_viewSize / m_zoom;
    m_left = -halfSize + camera_pos.x();
    m_right = halfSize + camera_pos.x();
    m_bottom = -halfSize + camera_pos.y();
    m_top = halfSize + camera_pos.y();
}

void CameraCore2D::applyToOpenGL()
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float aspect = viewport[2] / (float)viewport[3];

    // Вычисляем половину размера видимой области
    float halfSize = m_viewSize / m_zoom;

    // границы с учетом позиции камеры и соотношения сторон
    float left = -halfSize * aspect + camera_pos.x();
    float right = halfSize * aspect + camera_pos.x();
    float bottom = -halfSize + camera_pos.y();
    float top = halfSize + camera_pos.y();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(left, right, bottom, top, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
