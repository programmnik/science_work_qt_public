#include "GLWidget.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <cmath>
#include <QMatrix4x4>
#include "../core/constants.h"

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , zoom(1.0f)
    , m_fpsController(nullptr)
{
    cameraPoint = new Point(CameraConfig::DEFAULT_X,
                            CameraConfig::DEFAULT_Y,
                            CameraConfig::DEFAULT_Z,
                            CameraConfig::DEFAULT_ROT_X,
                            CameraConfig::DEFAULT_ROT_Y,
                            CameraConfig::DEFAULT_ROT_Z);
    camera = new CameraCore();
    camera->setPoint(cameraPoint);
    plane = new Plane();
    sphere = new Sphere();
    sphere->setPosition(0, 0, 0.5f);
    sphere->setRadius(0.5f);

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // Создаем FPS контроллер на куче
    m_fpsController = new FPSController(this);
    m_fpsController->setUpdateCallback([this]() {
        onUpdate();
    });
    m_fpsController->setMoveSpeed(CameraConfig::MOVE_SPEED);
    m_fpsController->setRotationSpeed(CameraConfig::ROTATION_SPEED);
    m_fpsController->setMaxSpeedMultiplier(5.0f);
    m_fpsController->setSpeedMultiplierStep(0.1f);
    m_fpsController->start();
}

GLWidget::~GLWidget()
{
    if (m_fpsController) {
        m_fpsController->stop();
        delete m_fpsController;
        m_fpsController = nullptr;
    }

    delete cameraPoint;
    delete camera;
    delete plane;
    delete sphere;
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

void GLWidget::onUpdate()
{
    if (!m_fpsController) return;

    // Получаем накопленный ввод от контроллера
    float rotX = m_fpsController->getAccumulatedRotX();
    float rotY = m_fpsController->getAccumulatedRotY();
    float moveForward = m_fpsController->getAccumulatedMoveForward();
    float moveRight = m_fpsController->getAccumulatedMoveRight();
    float moveUp = m_fpsController->getAccumulatedMoveUp();
    float speedMult = m_fpsController->getSpeedMultiplier();

    // Применяем ввод с ускорением
    if (rotX != 0.0f || rotY != 0.0f) {
        camera->rotate(rotX * speedMult, rotY * speedMult, 0);
    }

    if (moveForward != 0.0f || moveRight != 0.0f || moveUp != 0.0f) {
        camera->move(moveForward * speedMult,
                     moveRight * speedMult,
                     moveUp * speedMult);
    }

    // Очищаем накопленный ввод
    m_fpsController->clearAllInput();

    // Если нужно рисовать - обновляем
    if (m_fpsController->shouldRender()) {
        update();
    }
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = width() / (float)height();
    float fov = 60.0f / zoom;
    if (fov < 10.0f) fov = 10.0f;
    if (fov > 120.0f) fov = 120.0f;

    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    float top = nearPlane * tan(fov * M_PI / 360.0f);
    float bottom = -top;
    float right = top * aspect;
    float left = -right;

    glFrustum(left, right, bottom, top, nearPlane, farPlane);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    camera->applyToOpenGL();

    // Создаем QPainter для подписей
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Рисуем плоскость с подписями
    plane->draw(*cameraPoint, camera->getCameraFront(), &painter, width(), height());

    painter.end();

    sphere->draw(*cameraPoint);
    sphere->drawPoints(camera, width(), height(), zoom);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

Raycaster::FrustumParams GLWidget::getFrustumParams() const
{
    Raycaster::FrustumParams params;
    params.aspect = width() / (float)height();
    params.fov = 60.0f / zoom;
    if (params.fov < 10.0f) params.fov = 10.0f;
    if (params.fov > 120.0f) params.fov = 120.0f;

    params.nearPlane = 0.1f;
    params.farPlane = 1000.0f;
    params.top = params.nearPlane * tan(params.fov * M_PI / 360.0f);
    params.bottom = -params.top;
    params.right = params.top * params.aspect;
    params.left = -params.right;

    return params;
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        // 1. Получаем параметры frustum
        Raycaster::FrustumParams frustum = getFrustumParams();

        // 2. Создаем луч (начало на near plane)
        Raycaster::Ray ray = Raycaster::screenToRay(
            camera->getCameraPos(),
            camera->getCameraFront(),
            camera->getCameraRight(),
            camera->getCameraUp(),
            event->pos().x(), event->pos().y(),
            width(), height(),
            frustum
            );

        // 3. Проверяем пересечение со сферой
        QVector3D hitPoint;
        if (sphere->intersectsRay(ray.origin, ray.direction, hitPoint)) {
            // Сфера сама добавляет точку
            sphere->addPoint(hitPoint);
            update();
            qDebug() << "Точка добавлена в мировых координатах:" << hitPoint;
        }

        return;
    }
    if (event->button() == Qt::MiddleButton) {
        lastMousePos = event->pos();
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MiddleButton) {
        QPoint delta = event->pos() - lastMousePos;
        float rotX = delta.y() * MouseConfig::SENSITIVITY;
        float rotY = delta.x() * MouseConfig::SENSITIVITY;

        if (m_fpsController) {
            m_fpsController->addRotation(rotX, rotY, 0);
        }
        lastMousePos = event->pos();
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
    if (!m_fpsController) return;

    uint scanCode = event->nativeScanCode();

    // Передаем состояние клавиш в контроллер для удержания
    m_fpsController->setKeyState(scanCode, true);

    // Для мгновенного отклика добавляем одно нажатие
    float rotSpeed = CameraConfig::ROTATION_SPEED;
    float moveSpeed = CameraConfig::MOVE_SPEED;

    switch (scanCode) {
    case 113:   m_fpsController->addRotation(0, -rotSpeed, 0); break; // Left
    case 114:   m_fpsController->addRotation(0, rotSpeed, 0); break;  // Right
    case 111:   m_fpsController->addRotation(-rotSpeed, 0, 0); break; // Up
    case 116:   m_fpsController->addRotation(rotSpeed, 0, 0); break;  // Down
    case 25:    m_fpsController->addMovement(moveSpeed, 0, 0); break; // W
    case 39:    m_fpsController->addMovement(-moveSpeed, 0, 0); break;// S
    case 38:    m_fpsController->addMovement(0, -moveSpeed, 0); break;// A
    case 40:    m_fpsController->addMovement(0, moveSpeed, 0); break; // D
    case 24:    m_fpsController->addMovement(0, 0, moveSpeed); break; // Q
    case 52:    m_fpsController->addMovement(0, 0, -moveSpeed); break;// Z
    default:
        QOpenGLWidget::keyPressEvent(event);
        return;
    }
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (m_fpsController) {
        uint scanCode = event->nativeScanCode();
        m_fpsController->setKeyState(scanCode, false);
    }
    QOpenGLWidget::keyReleaseEvent(event);
}
