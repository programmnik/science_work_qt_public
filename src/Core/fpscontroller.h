#ifndef FPSCONTROLLER_H
#define FPSCONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <functional>

class FPSController : public QObject
{
    Q_OBJECT

public:
    explicit FPSController(QObject *parent = nullptr);
    ~FPSController();

    // Настройка
    void setTargetFPS(int fps);
    void setUpdateCallback(std::function<void()> callback);
    void setMaxSpeedMultiplier(float maxMult) { m_maxSpeedMultiplier = maxMult; }
    void setSpeedMultiplierStep(float step) { m_speedMultiplierStep = step; }
    void setMaxFrameSkip(int maxSkip) { m_maxFrameSkip = maxSkip; }
    void setMoveSpeed(float speed) { m_moveSpeed = speed; }
    void setRotationSpeed(float speed) { m_rotationSpeed = speed; }
    void setMode2D(bool isMode2D){ m_mode2d = isMode2D; }

    // Управление
    void start();
    void stop();
    bool isActive() const { return m_timer && m_timer->isActive(); }

    // Накопление ввода
    void addRotation(float dx, float dy, float dz = 0.0f);
    void addMovement(float forward, float right, float up);

    // Для клавиш (удержание)
    void setKeyState(uint scanCode, bool pressed);
    void clearAllInput();

    // Получение накопленных данных
    float getAccumulatedRotX() const { return m_accumulatedRotX; }
    float getAccumulatedRotY() const { return m_accumulatedRotY; }
    float getAccumulatedRotZ() const { return m_accumulatedRotZ; }
    float getAccumulatedMoveForward() const { return m_accumulatedMoveForward; }
    float getAccumulatedMoveRight() const { return m_accumulatedMoveRight; }
    float getAccumulatedMoveUp() const { return m_accumulatedMoveUp; }
    float getSpeedMultiplier() const { return m_speedMultiplier; }
    int getSkippedFrames() const { return m_skippedFrames; }
    float getCurrentFPS() const { return m_currentFPS; }
    bool shouldRender() const { return m_shouldRender; }

private slots:
    void processFrame();

private:
    void updateFPS();
    void process2DEvent();
    void process3DEvent();

    // Таймеры
    QTimer* m_timer;
    QElapsedTimer m_frameTimer;
    QElapsedTimer m_fpsTimer;

    // Настройки
    int m_targetFPS;
    float m_targetFrameTimeMs;
    float m_maxSpeedMultiplier;
    float m_speedMultiplierStep;
    int m_maxFrameSkip;
    float m_moveSpeed;
    float m_rotationSpeed;

    // Состояние
    int m_frameCount;
    int m_skippedFrames;
    float m_currentFPS;
    float m_speedMultiplier;
    bool m_shouldRender;
    int m_frameSkipCounter;

    // Накопленный ввод
    float m_accumulatedRotX;
    float m_accumulatedRotY;
    float m_accumulatedRotZ;
    float m_accumulatedMoveForward;
    float m_accumulatedMoveRight;
    float m_accumulatedMoveUp;
    bool m_hasInput;

    // Состояние клавиш
    struct KeyState {
        bool w = false, s = false, a = false, d = false;
        bool q = false, z = false;
        bool left = false, right = false, up = false, down = false;
    } m_keys;

    // Callback для обновления
    std::function<void()> m_updateCallback;

    bool m_mode2d;
};

#endif // FPSCONTROLLER_H
