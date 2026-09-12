#ifndef CAMERACORE3D_H
#define CAMERACORE3D_H


#include "camera.h"

class CameraCore3D : public Camera
{
public:
    CameraCore3D(): Camera() {}
    inline ~CameraCore3D() {}

    void applyToOpenGL() override;
    void move(float forward, float right, float up) override;
    void rotate(float dx, float dy, float dz) override;
    bool processEvent(uint scanCode, FPSController& fpsController) override;

    enum RotationMode {
        ModeLocal = 0,      // Локальное вращение
        ModeFixedYaw = 1    // Фиксированная ось Y для yaw
    };

    void setRotationMode(RotationMode mode) { m_rotationMode = mode; }
    void changeRotationMode() { m_rotationMode = m_rotationMode == ModeLocal ? ModeFixedYaw : ModeLocal; }
    RotationMode getRotationMode() const { return m_rotationMode; }

private:
    RotationMode m_rotationMode = ModeFixedYaw;

    void rotateLocal(float dx, float dy, float dz);
    void rotateFixedYaw(float dx, float dy, float dz);
};

#endif // CAMERACORE3D_H
