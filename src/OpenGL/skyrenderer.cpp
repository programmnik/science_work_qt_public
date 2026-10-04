#include "skyrenderer.h"
#include <QDebug>
#include <QFile>
#include <QString>

static void initSkyResources()
{
    Q_INIT_RESOURCE(shaders);
}

// Куб: 36 вершин, только позиции. Позиция = направление из центра.
static const float kSkyVertices[] = {
    // +X
    1,-1,-1,   1, 1,-1,   1, 1, 1,
    1,-1,-1,   1, 1, 1,   1,-1, 1,
    // -X
    -1,-1,-1,  -1, 1,-1,  -1, 1, 1,
    -1,-1,-1,  -1, 1, 1,  -1,-1, 1,
    // +Y
    -1, 1,-1,   1, 1,-1,   1, 1, 1,
    -1, 1,-1,   1, 1, 1,  -1, 1, 1,
    // -Y
    -1,-1,-1,   1,-1,-1,   1,-1, 1,
    -1,-1,-1,   1,-1, 1,  -1,-1, 1,
    // +Z
    -1,-1, 1,   1,-1, 1,   1, 1, 1,
    -1,-1, 1,   1, 1, 1,  -1, 1, 1,
    // -Z
    -1,-1,-1,   1,-1,-1,   1, 1,-1,
    -1,-1,-1,   1, 1,-1,  -1, 1,-1,
};

SkyRenderer::SkyRenderer() = default;

SkyRenderer::~SkyRenderer()
{
    destroy();
}

void SkyRenderer::initialize()
{
    if (m_initialized) return;
    initializeOpenGLFunctions();

    initSkyResources();

    buildShaders();
    buildGeometry();

    m_initialized = true;
}

static QString loadTextResource(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[SkyRenderer] Cannot open resource:" << path;
        return {};
    }
    return QString::fromUtf8(f.readAll());
}

void SkyRenderer::buildShaders()
{
    const QString vertSrc = loadTextResource(":/shaders/sky.vert");
    const QString fragSrc = loadTextResource(":/shaders/sky.frag");

    m_program = new QOpenGLShaderProgram();

    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertSrc)) {
        qDebug() << "[SkyRenderer] Vertex shader error:" << m_program->log();
    }
    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragSrc)) {
        qDebug() << "[SkyRenderer] Fragment shader error:" << m_program->log();
    }
    if (!m_program->link()) {
        qDebug() << "[SkyRenderer] Program link error:" << m_program->log();
    }
}

void SkyRenderer::buildGeometry()
{
    m_vao = new QOpenGLVertexArrayObject();
    m_vao->create();
    m_vao->bind();

    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_vbo->create();
    m_vbo->bind();
    m_vbo->allocate(kSkyVertices, sizeof(kSkyVertices));

    m_program->bind();
    m_program->enableAttributeArray(0);
    m_program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(float));
    m_program->release();

    m_vbo->release();
    m_vao->release();
}

void SkyRenderer::render(const QMatrix4x4& projection,
                         const QMatrix4x4& view,
                         float time,
                         const SkyParams& params)
{
    if (!m_initialized) return;

    // Сохраняем состояние, которое трогаем
    GLboolean depthWasEnabled = glIsEnabled(GL_DEPTH_TEST);
    GLboolean cullWasEnabled  = glIsEnabled(GL_CULL_FACE);
    GLboolean blendWasEnabled = glIsEnabled(GL_BLEND);

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    m_program->bind();
    m_program->setUniformValue("uProjection",    projection);
    m_program->setUniformValue("uView",          view);
    m_program->setUniformValue("uTopColor",      params.topColor);
    m_program->setUniformValue("uHorizonColor",  params.horizonColor);
    m_program->setUniformValue("uBottomColor",   params.bottomColor);
    m_program->setUniformValue("uHorizonY",      params.horizonY);
    m_program->setUniformValue("uTime",          time);
    m_program->setUniformValue("uStarDensity",   params.starDensity);
    m_program->setUniformValue("uStarBrightness",params.starBrightness);
    m_program->setUniformValue("uMilkyWay",      params.milkyWay);
    m_program->setUniformValue("uStarColor",     params.starColor);
    m_program->setUniformValue("uMilkyWayColor", params.milkyWayColor);

    m_vao->bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    m_vao->release();

    m_program->release();

    // Восстанавливаем
    glDepthMask(GL_TRUE);
    if (depthWasEnabled) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (cullWasEnabled)  glEnable(GL_CULL_FACE);  else glDisable(GL_CULL_FACE);
    if (blendWasEnabled) glEnable(GL_BLEND);      else glDisable(GL_BLEND);
}

void SkyRenderer::destroy()
{
    if (m_vbo) { m_vbo->destroy(); delete m_vbo; m_vbo = nullptr; }
    if (m_vao) { m_vao->destroy(); delete m_vao; m_vao = nullptr; }
    if (m_program) { delete m_program; m_program = nullptr; }
    m_initialized = false;
}
