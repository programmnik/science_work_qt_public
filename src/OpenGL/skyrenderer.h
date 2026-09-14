#ifndef SKYRENDERER_H
#define SKYRENDERER_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QVector3D>
#include <QColor>

// Параметры неба — то, что можно крутить извне
struct SkyParams
{
    QVector3D topColor      = QVector3D(0.010f, 0.012f, 0.030f); // зенит
    QVector3D horizonColor  = QVector3D(0.030f, 0.035f, 0.075f); // горизонт
    QVector3D bottomColor   = QVector3D(0.020f, 0.025f, 0.060f); // низ
    float     horizonY      = 0.0f;

    float     starDensity   = 0.06f;   // доля ячеек со звёздами
    float     starBrightness= 1.2f;    // общая яркость
    float     milkyWay      = 1.0f;   // интенсивность Млечного Пути

    // Цвета звёзд и Млечного Пути (можно менять)
    QVector3D starColor     = QVector3D(1.0f, 0.97f, 0.9f);
    QVector3D milkyWayColor = QVector3D(0.55f, 0.6f, 0.85f);
};

class SkyRenderer : protected QOpenGLFunctions_3_3_Core
{
public:
    SkyRenderer();
    ~SkyRenderer();

    // Вызвать один раз после initializeOpenGLFunctions() в initializeGL()
    void initialize();
    void destroy();

    // Вызвать каждый кадр в draw3DScene() ПОСЛЕ установки projection
    // и ДО отрисовки сцены.
    //   projection — QMatrix4x4::perspective(...)
    //   view       — view-матрица камеры (без трансляции)
    //   time       — секунды (для мерцания), можно 0 если не нужно
    void render(const QMatrix4x4& projection,
                const QMatrix4x4& view,
                float time,
                const SkyParams& params = SkyParams());

    // Геттер/сеттер параметров — если хочешь менять из GLWidget
    SkyParams&       params()       { return m_params; }
    const SkyParams& params() const { return m_params; }

private:
    void buildGeometry();
    void buildShaders();

    QOpenGLShaderProgram*     m_program = nullptr;
    QOpenGLBuffer*            m_vbo     = nullptr;
    QOpenGLVertexArrayObject* m_vao     = nullptr;

    SkyParams m_params;
    bool m_initialized = false;
};

#endif // SKYRENDERER_H
