#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "UI/Camera/camera.h"
#include "UI/Camera/cameracore3d.h"
#include "UI/Camera/cameracore2d.h"
#include "UI/Plane/plane3d.h"
#include "UI/Plane/plane2d.h"
#include "UI/Point/point.h"
#include "UI/Sphere/sphere.h"
#include "Core/fpscontroller.h"
#include "Core/raycaster.h"
#include "skyrenderer.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

    void setMode2D(bool enabled);
    bool isMode2D() const { return m_mode2D; }

signals:
    void modeChanged(bool is2D);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void onUpdate();

    SkyRenderer*  m_skyRenderer = nullptr;
    QElapsedTimer m_skyTimer;

    // GLuint m_skyVbo = 0;

    void draw2DScene();
    void draw3DScene();

    Point* cameraPoint;

    CameraCore3D* camera3d;
    CameraCore2D* camera2d;
    Camera* m_currentCamera;

    Plane3d* plane3d;
    Plane2d* plane2d;
    Sphere* sphere;

    QPoint lastMousePos;

    FPSController* m_fpsController;
    Raycaster::FrustumParams getFrustumParams() const;

    bool m_mode2D;
};

#endif // GLWIDGET_H
