#include "GLWidget.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <cmath>
#include <QMatrix4x4>
#include "constants.h"

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , zoom(1.0f)
{
    cameraPoint = new Point(CameraConfig::DEFAULT_X,
                            CameraConfig::DEFAULT_Y,
                            CameraConfig::DEFAULT_Z,
                            0, 0, 0);
    camera = new CameraCore();
    camera->setPoint(cameraPoint);
    plane = new Plane();

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

GLWidget::~GLWidget()
{
    delete plane;
    delete camera;
    delete cameraPoint;
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(Colors::BACKGROUND_R,
                 Colors::BACKGROUND_G,
                 Colors::BACKGROUND_B,
                 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Расчет frustum на основе fov, aspect ratio и zoom
    float aspect = width() / (float)height();
    float fov = 60.0f / zoom;
    if (fov < 10.0f) fov = 10.0f;
    if (fov > 120.0f) fov = 120.0f;

    // Конвертация fov в размеры frustum
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    float top = nearPlane * tan(fov * M_PI / 360.0f);  // fov/2 в радианах
    float bottom = -top;
    float right = top * aspect;
    float left = -right;

    // Установка frustum (используем glFrustum)
    glFrustum(left, right, bottom, top, nearPlane, farPlane);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    camera->applyToOpenGL();
    plane->draw(*cameraPoint);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        lastMousePos = event->pos();
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MiddleButton) {
        QPoint delta = event->pos() - lastMousePos;
        camera->rotate(delta.y() * 0.01f, delta.x() * 0.01f, 0);
        lastMousePos = event->pos();
        update();
    }
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    zoom *= (1.0f - event->angleDelta().y() / 1200.0f);
    if (zoom < MouseConfig::MIN_ZOOM) zoom = MouseConfig::MIN_ZOOM;
    if (zoom > MouseConfig::MAX_ZOOM) zoom = MouseConfig::MAX_ZOOM;
    update();
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    float move_speed = CameraConfig::MOVE_SPEED;
    float rotation_speed = CameraConfig::ROTATION_SPEED;
    switch (event->key()) {
        case Qt::Key_W: camera->move(-move_speed, 0, 0); break;
        case Qt::Key_S: camera->move(move_speed, 0, 0); break;
        case Qt::Key_A: camera->move(0, -move_speed, 0); break;
        case Qt::Key_D: camera->move(0, move_speed, 0); break;
        case Qt::Key_Q: camera->move(0, 0, move_speed); break;
        case Qt::Key_Z: camera->move(0, 0, -move_speed); break;
        case Qt::Key_Left: camera->rotate(0, -rotation_speed, 0); break;
        case Qt::Key_Right: camera->rotate(0, rotation_speed, 0); break;
        case Qt::Key_Up: camera->rotate(-rotation_speed, 0, 0); break;
        case Qt::Key_Down: camera->rotate(rotation_speed, 0, 0); break;
        default: return;
    }
    update();
}
