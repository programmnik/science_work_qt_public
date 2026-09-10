#include "fpscontroller.h"
#include <QKeyEvent>
#include <algorithm>

FPSController::FPSController(QObject *parent)
    : QObject(parent)
    , m_timer(nullptr)
    , m_targetFPS(60)
    , m_targetFrameTimeMs(1000.0f / 60)
    , m_maxSpeedMultiplier(5.0f)
    , m_speedMultiplierStep(0.1f)
    , m_maxFrameSkip(5)
    , m_moveSpeed(1.0f)
    , m_rotationSpeed(1.0f)
    , m_frameCount(0)
    , m_skippedFrames(0)
    , m_currentFPS(0.0f)
    , m_speedMultiplier(1.0f)
    , m_shouldRender(false)
    , m_frameSkipCounter(0)
    , m_accumulatedRotX(0.0f)
    , m_accumulatedRotY(0.0f)
    , m_accumulatedRotZ(0.0f)
    , m_accumulatedMoveForward(0.0f)
    , m_accumulatedMoveRight(0.0f)
    , m_accumulatedMoveUp(0.0f)
    , m_hasInput(false)
    , m_updateCallback(nullptr)
    , m_mode2d(false)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &FPSController::processFrame);
}

FPSController::~FPSController()
{
    stop();
    // m_timer удалится автоматически (parent = this)
}

void FPSController::setTargetFPS(int fps)
{
    m_targetFPS = fps;
    m_targetFrameTimeMs = 1000.0f / fps;

    if (isActive()) {
        stop();
        start();
    }
}

void FPSController::setUpdateCallback(std::function<void()> callback)
{
    m_updateCallback = callback;
}

void FPSController::start()
{
    if (m_timer && !m_timer->isActive()) {
        m_frameTimer.start();
        m_fpsTimer.start();
        m_timer->start(std::max(1, static_cast<int>(m_targetFrameTimeMs)));
    }
}

void FPSController::stop()
{
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
    }
}

void FPSController::addRotation(float dx, float dy, float dz)
{
    m_accumulatedRotX += dx;
    m_accumulatedRotY += dy;
    // m_accumulatedRotZ += dz;
    m_hasInput = true;
}

void FPSController::addMovement(float forward, float right, float up)
{
    m_accumulatedMoveForward += forward;
    m_accumulatedMoveRight += right;
    m_accumulatedMoveUp += up;
    m_hasInput = true;
}

void FPSController::setKeyState(uint scanCode, bool pressed)
{
    switch (scanCode) {
    case 25:  m_keys.w = pressed; break;   // W
    case 39:  m_keys.s = pressed; break;   // S
    case 38:  m_keys.a = pressed; break;   // A
    case 40:  m_keys.d = pressed; break;   // D
    case 113: m_keys.left = pressed; break;   // Left
    case 114: m_keys.right = pressed; break;  // Right
    case 111: m_keys.up = pressed; break;     // Up
    case 116: m_keys.down = pressed; break;   // Down
    default: break;
    }

    if(!m_mode2d){
        switch (scanCode) {
            case 24:  m_keys.q = pressed; break;   // Q
            case 52:  m_keys.z = pressed; break;   // Z
            default: break;
        }
    }
}

void FPSController::clearAllInput()
{
    m_accumulatedRotX = 0.0f;
    m_accumulatedRotY = 0.0f;
    m_accumulatedRotZ = 0.0f;
    m_accumulatedMoveForward = 0.0f;
    m_accumulatedMoveRight = 0.0f;
    m_accumulatedMoveUp = 0.0f;
    m_hasInput = false;
    m_speedMultiplier = 1.0f;
}

void FPSController::process2DEvent(){
    if (m_keys.w) m_accumulatedMoveForward += m_moveSpeed;
    if (m_keys.s) m_accumulatedMoveForward -= m_moveSpeed;
    if (m_keys.a) m_accumulatedMoveRight -= m_moveSpeed;
    if (m_keys.d) m_accumulatedMoveRight += m_moveSpeed;
    if (m_keys.left) m_accumulatedMoveRight -= m_moveSpeed;
    if (m_keys.right) m_accumulatedMoveRight += m_moveSpeed;
    if (m_keys.up) m_accumulatedMoveForward += m_moveSpeed;
    if (m_keys.down) m_accumulatedMoveForward -= m_moveSpeed;

}

void FPSController::process3DEvent(){
    if (m_keys.w) m_accumulatedMoveForward += m_moveSpeed;
    if (m_keys.s) m_accumulatedMoveForward -= m_moveSpeed;
    if (m_keys.a) m_accumulatedMoveRight -= m_moveSpeed;
    if (m_keys.d) m_accumulatedMoveRight += m_moveSpeed;
    if (m_keys.q) m_accumulatedMoveUp += m_moveSpeed;
    if (m_keys.z) m_accumulatedMoveUp -= m_moveSpeed;
    if (m_keys.left) m_accumulatedRotY -= m_rotationSpeed;
    if (m_keys.right) m_accumulatedRotY += m_rotationSpeed;
    if (m_keys.up) m_accumulatedRotX -= m_rotationSpeed;
    if (m_keys.down) m_accumulatedRotX += m_rotationSpeed;

}

void FPSController::processFrame()
{
    updateFPS();

    // 1. Обрабатываем удержание клавиш
    if(m_mode2d) process2DEvent();
    else process3DEvent();

    if (m_keys.w || m_keys.s || m_keys.a || m_keys.d ||
        m_keys.q || m_keys.z || m_keys.left || m_keys.right ||
        m_keys.up || m_keys.down) {
        m_hasInput = true;
    }

    // 2. Проверяем, нужно ли пропустить кадр
    m_shouldRender = true;
    float elapsed = m_frameTimer.elapsed();

    if (elapsed < m_targetFrameTimeMs / 2.0f && m_frameCount > 0) {
        m_frameSkipCounter++;
        if (m_frameSkipCounter < m_maxFrameSkip) {
            if (m_hasInput) {
                m_speedMultiplier = std::min(
                    m_speedMultiplier + m_speedMultiplierStep,
                    m_maxSpeedMultiplier
                    );
            }
            m_skippedFrames++;
            m_shouldRender = false;
            return;
        }
    }
    m_frameSkipCounter = 0;

    // 3. Если есть накопленный ввод - применяем его
    if (m_hasInput) {
        // Данные уже накоплены, сбрасываем флаг
        // Внешний код получит данные через getAccumulated* методы
        m_hasInput = false;
    }

    // 4. Вызываем callback для обновления
    if (m_updateCallback) {
        m_updateCallback();
    }

    m_skippedFrames = 0;
    m_frameTimer.restart();
}

void FPSController::updateFPS()
{
    m_frameCount++;

    static int fpsFrameCount = 0;

    if (fpsFrameCount == 0) {
        m_fpsTimer.start();
    }

    fpsFrameCount++;

    if (m_fpsTimer.elapsed() >= 1000) {
        m_currentFPS = fpsFrameCount * 1000.0f / m_fpsTimer.elapsed();
        fpsFrameCount = 0;
    }
}
