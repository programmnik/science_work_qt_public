#include "GLWidget.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <cmath>
#include <QMatrix4x4>
#include "Core/constants.h"
#include "Data/objectmanager.h"
#include "Data/sceneobject.h"

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_fpsController(nullptr)
    , m_mode2D(false)
    , m_currentCamera(nullptr)
{
    cameraPoint = new Point(CameraConfig::DEFAULT_X,
                            CameraConfig::DEFAULT_Y,
                            CameraConfig::DEFAULT_Z,
                            CameraConfig::DEFAULT_ROT_X,
                            CameraConfig::DEFAULT_ROT_Y,
                            CameraConfig::DEFAULT_ROT_Z);
    camera3d = new CameraCore3D();
    camera3d->setPoint(cameraPoint);
    camera3d->setMoveSpeed(CameraConfig::MOVE_SPEED);
    camera3d->setRotSpeed(CameraConfig::ROTATION_SPEED);
    camera3d->setZoomRange(MouseConfig::MIN_ZOOM, MouseConfig::MAX_ZOOM);

    camera2d = new CameraCore2D();
    camera2d->setViewSize(5.0f);
    camera2d->setMoveSpeed(CameraConfig::MOVE_SPEED);

    m_currentCamera = camera3d;

    plane3d = new Plane3d();
    plane2d = new Plane2d();
    sphere = new Sphere();
    sphere->setPosition(0, 0, 0.5f);
    sphere->setRadius(0.5f);

    auto& manager = ObjectManager::instance();

    auto sphereObj = std::make_shared<SceneObject>(SceneObject::TypeSphere, "Сфера Римана");
    sphereObj->setColor(QColor(30, 144, 255));
    manager.addObject(sphereObj);

    auto planeObj = std::make_shared<SceneObject>(SceneObject::TypePlane, "Комплексная плоскость");
    planeObj->setColor(QColor(100, 200, 200));
    manager.addObject(planeObj);

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
    m_fpsController->setMode2D(false);
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
    delete camera3d;
    delete camera2d;
    delete plane3d;
    delete plane2d;
    delete sphere;
}

void GLWidget::setMode2D(bool enabled)
{
    m_mode2D = enabled;
    m_fpsController->setMode2D(enabled);
    m_currentCamera = enabled ? static_cast<Camera*>(camera2d) : static_cast<Camera*>(camera3d);
    emit modeChanged(enabled);
    update();
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
        if(!m_mode2D) camera3d->rotate(rotX * speedMult, rotY * speedMult, 0);
    }

    if (moveForward != 0.0f || moveRight != 0.0f || moveUp != 0.0f) {
        if(m_mode2D){
            camera2d->move(moveRight * speedMult,
                           moveForward * speedMult,
                           0.0f);
        }
        else{
            camera3d->move(moveForward * speedMult,
                           moveRight * speedMult,
                           moveUp * speedMult);
        }
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

    if (m_mode2D) {
        draw2DScene();
    } else {
        draw3DScene();
    }
}

void GLWidget::draw3DScene()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float zoom = camera3d->getZoom();

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

    camera3d->applyToOpenGL();

    // Создаем QPainter для подписей
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Рисуем плоскость с подписями
    plane3d->draw(camera3d, &painter, width(), height());

    painter.end();

    sphere->draw(*cameraPoint);
    sphere->drawPoints(camera3d, width(), height(), zoom);
}

void GLWidget::draw2DScene()
{
    // Ортогональная проекция (вид сверху)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = width() / (float)height();
    float size = 5.0f;  // Размер видимой области

    if (aspect > 1.0f) {
        glOrtho(-size * aspect, size * aspect, -size, size, -1.0f, 1.0f);
    } else {
        glOrtho(-size, size, -size / aspect, size / aspect, -1.0f, 1.0f);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    camera2d->applyToOpenGL();

    // Создаем QPainter для подписей
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Рисуем плоскость (вид сверху)
    plane2d->draw(camera2d, &painter, width(), height());

    // TODO: рисовать точки и их проекции в 2D
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

Raycaster::FrustumParams GLWidget::getFrustumParams() const
{
    Raycaster::FrustumParams params;
    params.aspect = width() / (float)height();
    params.fov = 60.0f / camera3d->getZoom();
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
        if (m_mode2D) return;

        // 1. Получаем параметры frustum
        Raycaster::FrustumParams frustum = getFrustumParams();

        // 2. Создаем луч (начало на near plane)
        Raycaster::Ray ray = Raycaster::screenToRay(
            camera3d->getCameraPos(),
            camera3d->getCameraFront(),
            camera3d->getCameraRight(),
            camera3d->getCameraUp(),
            event->pos().x(), event->pos().y(),
            width(), height(),
            frustum
            );

        // 3. Проверяем пересечение со сферой
        QVector3D hitPoint;
        if (sphere->intersectsRay(ray.origin, ray.direction, hitPoint)) {
            sphere->addPoint(hitPoint);

            auto& manager = ObjectManager::instance();
            auto pointObj = std::make_shared<SceneObject>(
                SceneObject::TypePoint,
                QString("Точка (%1, %2, %3)").arg(hitPoint.x(), 2, 'f', 2)
                    .arg(hitPoint.y(), 2, 'f', 2)
                    .arg(hitPoint.z(), 2, 'f', 2)
                );
            pointObj->setColor(QColor(255, 255, 0));
            manager.addObject(pointObj);

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
    float factor = 1.0f - event->angleDelta().y() / 1200.0f;
    m_currentCamera->zoom(factor);
    update();
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if (!m_fpsController) return;

    uint scanCode = event->nativeScanCode();

    // Передаем состояние клавиш в контроллер для удержания
    m_fpsController->setKeyState(scanCode, true);

    if(!m_currentCamera->processEvent(scanCode, *m_fpsController)) QOpenGLWidget::keyPressEvent(event);
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (m_fpsController) {
        uint scanCode = event->nativeScanCode();
        m_fpsController->setKeyState(scanCode, false);
    }
    QOpenGLWidget::keyReleaseEvent(event);
}
