#include "GLWidget.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <cmath>
#include <QMatrix4x4>
#include "../core/constants.h"

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , zoom(1.0f)
    , m_frameCount(0)
    , m_currentFPS(0.0f)
    , m_speedMultiplier(1.0f)
    , m_skippedFrames(0)
    , m_accumulatedRotX(0.0f)
    , m_accumulatedRotY(0.0f)
    , m_accumulatedMoveForward(0.0f)
    , m_accumulatedMoveRight(0.0f)
    , m_accumulatedMoveUp(0.0f)
    , m_hasAccumulatedInput(false)
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

    // Настройка таймера обновления с лямбдой
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, [this]() {
        updateLoop();
    });
    m_updateTimer->start(1000 / FPSConfig::TARGET_FPS);

    m_frameTimer.start();
}

GLWidget::~GLWidget()
{
    delete plane;
    delete camera;
    delete cameraPoint;
    delete sphere;
    delete m_updateTimer;
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

void GLWidget::updateLoop()
{
    updateFPS();

    // Проверяем, нужно ли пропустить кадр
    static int frameSkipCounter = 0;
    float targetFrameTime = FPSConfig::UPDATE_INTERVAL_MS;
    float elapsed = m_frameTimer.elapsed();

    // Если прошло слишком мало времени, пропускаем кадр
    if (elapsed < targetFrameTime / 2.0f && m_frameCount > 0) {
        frameSkipCounter++;
        if (frameSkipCounter < FPSConfig::MAX_FRAME_SKIP) {
            // Пропускаем отрисовку, но накапливаем ввод
            if (m_hasAccumulatedInput) {
                // Увеличиваем множитель скорости
                m_speedMultiplier = std::min(
                    m_speedMultiplier + FPSConfig::SPEED_MULTIPLIER_STEP,
                    FPSConfig::MAX_SPEED_MULTIPLIER
                    );
            }
            m_skippedFrames++;
            return;
        }
    }
    frameSkipCounter = 0;

    // Если есть накопленный ввод, обрабатываем его с множителем
    if (m_hasAccumulatedInput) {
        processAccumulatedInput();
        m_hasAccumulatedInput = false;
        // Сбрасываем множитель скорости
        m_speedMultiplier = 1.0f;
    }

    // Отрисовка
    update();

    // Сброс счетчика пропущенных кадров
    m_skippedFrames = 0;
    m_frameTimer.restart();
}

void GLWidget::processAccumulatedInput()
{
    // Применяем накопленные движения с учетом множителя скорости
    float speedMult = m_speedMultiplier;

    // Вращение
    if (m_accumulatedRotX != 0.0f || m_accumulatedRotY != 0.0f) {
        camera->rotate(m_accumulatedRotX * speedMult,
                       m_accumulatedRotY * speedMult,
                       0);
    }

    // Движение
    if (m_accumulatedMoveForward != 0.0f ||
        m_accumulatedMoveRight != 0.0f ||
        m_accumulatedMoveUp != 0.0f) {
        camera->move(m_accumulatedMoveForward * speedMult,
                     m_accumulatedMoveRight * speedMult,
                     m_accumulatedMoveUp * speedMult);
    }

    // Сбрасываем накопления
    m_accumulatedRotX = 0.0f;
    m_accumulatedRotY = 0.0f;
    m_accumulatedMoveForward = 0.0f;
    m_accumulatedMoveRight = 0.0f;
    m_accumulatedMoveUp = 0.0f;
}

void GLWidget::updateFPS()
{
    m_frameCount++;

    static QElapsedTimer fpsTimer;
    static int fpsFrameCount = 0;

    if (fpsFrameCount == 0) {
        fpsTimer.start();
    }

    fpsFrameCount++;

    if (fpsTimer.elapsed() >= 1000) {
        m_currentFPS = fpsFrameCount * 1000.0f / fpsTimer.elapsed();
        fpsFrameCount = 0;

        // Отладочный вывод (можно убрать)
        // qDebug() << "FPS:" << m_currentFPS << "Skipped:" << m_skippedFrames << "Speed:" << m_speedMultiplier;
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
    plane->draw(*cameraPoint);
    sphere->draw(*cameraPoint);
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
        float rotX = delta.y() * MouseConfig::SENSITIVITY;
        float rotY = delta.x() * MouseConfig::SENSITIVITY;

        // Накопливаем вращение
        m_accumulatedRotX += rotX;
        m_accumulatedRotY += rotY;
        m_hasAccumulatedInput = true;

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
    float move_speed = CameraConfig::MOVE_SPEED;
    float rotation_speed = CameraConfig::ROTATION_SPEED;

    bool hasInput = false;

    switch (event->key()) {
    case Qt::Key_W:
        m_accumulatedMoveForward -= move_speed;
        hasInput = true;
        break;
    case Qt::Key_S:
        m_accumulatedMoveForward += move_speed;
        hasInput = true;
        break;
    case Qt::Key_A:
        m_accumulatedMoveRight -= move_speed;
        hasInput = true;
        break;
    case Qt::Key_D:
        m_accumulatedMoveRight += move_speed;
        hasInput = true;
        break;
    case Qt::Key_Q:
        m_accumulatedMoveUp += move_speed;
        hasInput = true;
        break;
    case Qt::Key_Z:
        m_accumulatedMoveUp -= move_speed;
        hasInput = true;
        break;
    case Qt::Key_Left:
        m_accumulatedRotY -= rotation_speed;
        hasInput = true;
        break;
    case Qt::Key_Right:
        m_accumulatedRotY += rotation_speed;
        hasInput = true;
        break;
    case Qt::Key_Up:
        m_accumulatedRotX -= rotation_speed;
        hasInput = true;
        break;
    case Qt::Key_Down:
        m_accumulatedRotX += rotation_speed;
        hasInput = true;
        break;
    default:
        QOpenGLWidget::keyPressEvent(event);
        return;
    }

    if (hasInput) {
        m_hasAccumulatedInput = true;
    }
}
