#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QElapsedTimer>
#include "../core/cameracore.h"
#include "../core/plane.h"
#include "../core/point.h"
#include "../core/sphere.h"

// Убираем Q_OBJECT - он не нужен, так как нет сигналов/слотов
class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    // Q_OBJECT  // <-- УДАЛИТЬ ЭТУ СТРОКУ

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

    // FPS управление
    QTimer* m_updateTimer;
    QElapsedTimer m_frameTimer;
    int m_frameCount;
    float m_currentFPS;
    float m_speedMultiplier;
    int m_skippedFrames;

    // Накопленные движения
    float m_accumulatedRotX;
    float m_accumulatedRotY;
    float m_accumulatedMoveForward;
    float m_accumulatedMoveRight;
    float m_accumulatedMoveUp;
    bool m_hasAccumulatedInput;

    void processAccumulatedInput();
    void updateFPS();
    void updateLoop();
};

#endif // GLWIDGET_H
