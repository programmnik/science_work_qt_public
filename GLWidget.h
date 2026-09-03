#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "src/core/cameracore.h"
#include "src/core/plane.h"
#include "src/core/point.h"
#include "src/core/sphere.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

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

private:
    Point* cameraPoint;
    CameraCore* camera;
    Plane* plane;
    Sphere* sphere;

    QPoint lastMousePos;
    float zoom;
};

#endif // GLWIDGET_H
