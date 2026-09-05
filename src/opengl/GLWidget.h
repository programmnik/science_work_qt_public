#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "../core/cameracore.h"
#include "../core/plane.h"
#include "../core/point.h"
#include "../core/sphere.h"
#include "../core/fpscontroller.h"
#include "../core/raycaster.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

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

    Point* cameraPoint;
    CameraCore* camera;
    Plane* plane;
    Sphere* sphere;

    QPoint lastMousePos;
    float zoom;

    FPSController* m_fpsController;

    Raycaster::FrustumParams getFrustumParams() const;
};

#endif // GLWIDGET_H
