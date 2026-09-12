#ifndef CAMERACORE2D_H
#define CAMERACORE2D_H

#include "camera.h"

class CameraCore2D : public Camera
{
public:
    CameraCore2D();
    inline ~CameraCore2D() {}

    // Реализация виртуальных методов
    void applyToOpenGL() override;
    void move(float dx, float dy, float dz) override;
    bool processEvent(uint scanCode, FPSController& fpsController) override;
    void zoom(float factor) override;

    // Дополнительные методы
    void setViewSize(float size) { m_viewSize = size; updateProjection(); }
    float getViewSize() const { return m_viewSize; }

private:
    void updateProjection();

    float m_viewSize = 5.0f;
    float m_left = -5.0f, m_right = 5.0f;
    float m_bottom = -5.0f, m_top = 5.0f;
};

#endif // CAMERACORE2D_H
