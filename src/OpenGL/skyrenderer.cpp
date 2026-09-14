#include "skyrenderer.h"
#include <QDebug>

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

// ---------- Вершинный шейдер ----------
static const char* kVertSrc = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
out vec3 vDir;

uniform mat4 uProjection;
uniform mat4 uView;   // view БЕЗ трансляции

void main()
{
    vDir = aPos;
    gl_Position = uProjection * uView * vec4(aPos, 1.0);
}
)";

// ---------- Фрагментный шейдер ----------
static const char* kFragSrc = R"(
#version 330 core

in vec3 vDir;
out vec4 fragColor;

uniform vec3  uTopColor;
uniform vec3  uHorizonColor;
uniform vec3  uBottomColor;
uniform float uHorizonY;

uniform float uTime;
uniform float uStarDensity;
uniform float uStarBrightness;
uniform float uMilkyWay;
uniform vec3  uStarColor;
uniform vec3  uMilkyWayColor;

// ===================== Хэши =====================
float hash11(float p) {
    p = fract(p * 0.1031);
    p *= p + 33.33;
    p *= p + p;
    return fract(p);
}

float hash13(vec3 p3) {
    p3 = fract(p3 * 0.1031);
    p3 += dot(p3, p3.zyx + 31.32);
    return fract((p3.x + p3.y) * p3.z);
}

vec3 hash33(vec3 p3) {
    p3 = fract(p3 * vec3(0.1031, 0.1030, 0.0973));
    p3 += dot(p3, p3.yxz + 33.33);
    return fract((p3.xxy + p3.yxx) * p3.zyx);
}

// ===================== Value noise 3D =====================
float noise3(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);

    float n000 = hash13(i + vec3(0,0,0));
    float n100 = hash13(i + vec3(1,0,0));
    float n010 = hash13(i + vec3(0,1,0));
    float n110 = hash13(i + vec3(1,1,0));
    float n001 = hash13(i + vec3(0,0,1));
    float n101 = hash13(i + vec3(1,0,1));
    float n011 = hash13(i + vec3(0,1,1));
    float n111 = hash13(i + vec3(1,1,1));

    float nx00 = mix(n000, n100, f.x);
    float nx10 = mix(n010, n110, f.x);
    float nx01 = mix(n001, n101, f.x);
    float nx11 = mix(n011, n111, f.x);

    float nxy0 = mix(nx00, nx10, f.y);
    float nxy1 = mix(nx01, nx11, f.y);

    return mix(nxy0, nxy1, f.z);
}

float fbm(vec3 p) {
    float v = 0.0;
    float a = 0.5;
    for (int i = 0; i < 5; ++i) {
        v += a * noise3(p);
        p = p * 2.03 + vec3(13.7, 7.3, 5.1);
        a *= 0.5;
    }
    return v;
}

// ===================== Градиент неба =====================
vec3 skyGradient(float y) {
    if (y >= uHorizonY) {
        float t = clamp((y - uHorizonY) / (1.0 - uHorizonY), 0.0, 1.0);
        return mix(uHorizonColor, uTopColor, t);
    } else {
        float t = clamp((uHorizonY - y) / (1.0 + uHorizonY), 0.0, 1.0);
        return mix(uHorizonColor, uBottomColor, t);
    }
}

// ===================== Звёзды =====================
float starsInCell(vec3 cell, vec3 dir, float cellSize) {
    vec3 starPos = (cell + hash33(cell)) * cellSize;
    float d = length(dir - normalize(starPos));

    float brightness = hash13(cell + 7.13);
    brightness = pow(brightness, 8.0);

    float size = mix(0.0008, 0.004, hash13(cell + 3.7));

    return smoothstep(size, 0.0, d) * brightness;
}

// ===================== Млечный Путь =====================
float milkyWay(vec3 dir) {
    vec3 mwNormal = normalize(vec3(0.3, 0.8, -0.5));
    float distToPlane = abs(dot(dir, mwNormal));
    float band = exp(-distToPlane * distToPlane * 12.0);
    float n = fbm(dir * 6.0);
    n = pow(n, 2.0);
    return band * n;
}

// ===================== main =====================
void main()
{
    vec3 dir = normalize(vDir);
    float z = dir.z;

    vec3 col = skyGradient(z);

    // Млечный Путь
    if (uMilkyWay > 0.0) {
        float mw = milkyWay(dir);
        col += uMilkyWayColor * mw * uMilkyWay * 0.25;
    }

    // Звёзды
    float horizonFade = smoothstep(-0.05, 0.35, z+0.95);

    const float CELL = 100.0;
    vec3 p = dir * CELL;
    vec3 cell = floor(p);

    float stars = 0.0;
    for (int dx = -1; dx <= 1; ++dx)
    for (int dy = -1; dy <= 1; ++dy)
    for (int dz = -1; dz <= 1; ++dz) {
        vec3 c = cell + vec3(dx, dy, dz);
        if (hash13(c + 1.7) > uStarDensity) continue;
        stars += starsInCell(c, dir, 1.0 / CELL);
    }

    // Мерцание
    float twinkle = 0.5 + 0.5 * sin(uTime * 3.0 + hash13(cell) * 6.2831);

    col += uStarColor * stars * uStarBrightness * twinkle; // '* horizonFade' при необходимости ограничить звёздное пространство

    col = max(col, vec3(0.0));
    fragColor = vec4(col, 1.0);
}
)";

// ===================== Реализация =====================

SkyRenderer::SkyRenderer() = default;

SkyRenderer::~SkyRenderer()
{
    destroy();
}

void SkyRenderer::initialize()
{
    if (m_initialized) return;
    initializeOpenGLFunctions();

    buildShaders();
    buildGeometry();

    m_initialized = true;
}

void SkyRenderer::buildShaders()
{
    m_program = new QOpenGLShaderProgram();

    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, kVertSrc)) {
        qDebug() << "[SkyRenderer] Vertex shader error:" << m_program->log();
    }
    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, kFragSrc)) {
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
