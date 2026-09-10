#include "plane.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "constants.h"
#include <cmath>
#include <QDebug>
#include <QFont>

Plane::Plane()
    : viewDistance(100.0f)
    , viewNumberDistance(30.0f)
    , stepNumber(1.0f)
    , stepX(1.0f)
    , stepY(1.0f)
{
}

void Plane::calculateVisibleRange(const Point &cameraPoint,
                                  float &minX, float &maxX,
                                  float &minY, float &maxY)
{
    float camX = cameraPoint.getX();
    float camY = cameraPoint.getY();

    minX = camX - viewDistance;
    maxX = camX + viewDistance;
    minY = camY - viewDistance;
    maxY = camY + viewDistance;
}

// ==================== ПРОЕКЦИЯ ====================

QPointF Plane::projectToScreen(const QVector3D& worldPos)
{
    GLdouble modelview[16];
    GLdouble projection[16];
    GLint viewport[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLdouble winX, winY, winZ;
    int result = gluProject(worldPos.x(), worldPos.y(), worldPos.z(),
                            modelview, projection, viewport,
                            &winX, &winY, &winZ);

    if (result == GL_FALSE || winZ >= 1.0f) {
        return QPointF(-1000, -1000);
    }

    return QPointF(winX, viewport[3] - winY);
}

bool Plane::isPointInFrontOfCamera(const QVector3D& point,
                                   const QVector3D& cameraPos,
                                   const QVector3D& cameraFront)
{
    QVector3D toPoint = point - cameraPos;
    float dot = QVector3D::dotProduct(toPoint, cameraFront);
    return dot > 0;
}

// ==================== ГЕНЕРАЦИЯ МАТРИЦЫ ПОДПИСЕЙ ====================

QVector<Plane::Label> Plane::generateLabels(const QVector3D& cameraPos,
                                            const QVector3D& cameraFront,
                                            int screenWidth, int screenHeight)
{
    QVector<Label> labels;

    if (m_labelMode == ModeNone) return labels;

    float camX = cameraPos.x();
    float camY = cameraPos.y();

    float minX = camX - viewNumberDistance;
    float maxX = camX + viewNumberDistance;
    float minY = camY - viewNumberDistance;
    float maxY = camY + viewNumberDistance;

    auto isVisible = [&](const QVector3D& pos) -> bool {
        if (!isPointInFrontOfCamera(pos, cameraPos, cameraFront)) return false;
        QPointF screenPos = projectToScreen(pos);
        return (screenPos.x() >= 0 && screenPos.x() <= screenWidth &&
                screenPos.y() >= 0 && screenPos.y() <= screenHeight);
    };

    auto addLabel = [&](const ComplexNumber& num, const QVector3D& pos, const QColor& color) {
        Label label;
        label.number = num;
        label.worldPos = pos;
        label.text = num.toString();
        label.color = color;
        label.fontSize = m_fontSize;
        labels.append(label);
    };

    // ============================================================
    // РЕЖИМ: только оси
    // ============================================================
    if (m_labelMode == ModeAxesOnly) {
        // ---- Реальная ось: (x, 0) ----
        // Рисуем все x от minX до maxX
        if(minY <= 0 && maxY >= 0){
            for (float x = std::floor(minX); x <= std::ceil(maxX); x += stepNumber) {
                if (std::abs(x) < 0.01f) continue;
                QVector3D pos(x, 0, 0);
                if (isVisible(pos)) {
                    addLabel({x, 0.0f}, pos, QColor(255, 100, 100));
                }
            }
        }

        // ---- Мнимая ось: (0, y) ----
        if(minX <= 0 && maxX >= 0){
            for (float y = std::floor(minY); y <= std::ceil(maxY); y += stepNumber) {
                if (std::abs(y) < 0.01f) continue;
                QVector3D pos(0, y, 0);
                if (isVisible(pos)) {
                    addLabel({0.0f, y}, pos, QColor(100, 255, 100));
                }
            }
        }

        // ---- Ноль: (0, 0) ----
        if(minY <= 0 && maxY >= 0 && minX <= 0 && maxX >= 0){
            QVector3D originPos(0, 0, 0);
            if (isVisible(originPos)) {
                addLabel({0.0f, 0.0f}, originPos, QColor(255, 255, 255));
            }
        }
    }

    // ============================================================
    // РЕЖИМ: все числа
    // ============================================================
    if (m_labelMode == ModeAll) {
        for (float x = std::floor(minX); x <= std::ceil(maxX); x += stepNumber) {
            for (float y = std::floor(minY); y <= std::ceil(maxY); y += stepNumber) {
                ComplexNumber num{x, y};

                // Пропускаем только если оба нуля (0 уже нарисован)
                if (num.isZero()) continue;

                QVector3D pos(x, y, 0);

                if (!isPointInFrontOfCamera(pos, cameraPos, cameraFront)) continue;

                QPointF screenPos = projectToScreen(pos);
                if (screenPos.x() < 0 || screenPos.x() > screenWidth ||
                    screenPos.y() < 0 || screenPos.y() > screenHeight) continue;

                Label label;
                label.number = num;
                label.worldPos = pos;
                label.text = num.toString();

                // Цвет зависит от положения
                if (num.isOnAxis()) {
                    if (std::abs(num.re) < 0.001f) {
                        label.color = QColor(100, 255, 100);  // Зеленый
                    } else {
                        label.color = QColor(255, 100, 100);  // Красный
                    }
                } else {
                    label.color = QColor(200, 200, 255, 180);  // Синий полупрозрачный
                }

                label.fontSize = m_fontSize;
                labels.append(label);
            }
        }
    }

    return labels;
}

// ==================== ОТРИСОВКА ====================

void Plane::drawGrid(const Point& cameraPoint)
{
    float minX, maxX, minY, maxY;
    calculateVisibleRange(cameraPoint, minX, maxX, minY, maxY);

    float startX = std::floor(minX / stepX) * stepX;
    float startY = std::floor(minY / stepY) * stepY;

    glColor3f(Colors::PLANE_R,
              Colors::PLANE_G,
              Colors::PLANE_B);

    glBegin(GL_LINES);
    for (float x = startX; x <= maxX; x += stepX) {
        if(std::abs(x) < 0.01f) continue;
        glVertex3f(x, minY, 0);
        glVertex3f(x, maxY, 0);
    }

    for (float y = startY; y <= maxY; y += stepY) {
        if(std::abs(y) < 0.01f) continue;
        glVertex3f(minX, y, 0);
        glVertex3f(maxX, y, 0);
    }
    glEnd();
}

void Plane::drawAxes(const Point& cameraPoint)
{
    float minX, maxX, minY, maxY;
    calculateVisibleRange(cameraPoint, minX, maxX, minY, maxY);

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    if(minY <= 0 && maxY >= 0){
        glColor3f(Colors::AXIS_X_R,
                  Colors::AXIS_X_G,
                  Colors::AXIS_X_B);
        glVertex3f(minX, 0, 0);
        glVertex3f(maxX, 0, 0);
    }

    if(minX <= 0 && maxX >= 0){
        glColor3f(Colors::AXIS_Y_R,
                  Colors::AXIS_Y_G,
                  Colors::AXIS_Y_B);
        glVertex3f(0, minY, 0);
        glVertex3f(0, maxY, 0);
    }
    glEnd();
    glLineWidth(1.0f);
}

// ==================== ГЛАВНЫЙ МЕТОД ====================

void Plane::draw(const Point& cameraPoint,
                 const QVector3D& cameraFront,
                 QPainter* painter,
                 int screenWidth, int screenHeight)
{
    // 1. Рисуем сетку и оси через OpenGL
    drawGrid(cameraPoint);
    drawAxes(cameraPoint);

    if (!painter) return;

    QVector3D cameraPos(cameraPoint.getX(), cameraPoint.getY(), cameraPoint.getZ());

    // 2. Генерируем матрицу подписей
    QVector<Label> labels = generateLabels(cameraPos, cameraFront, screenWidth, screenHeight);

    // 3. Настройка шрифта
    QFont font = painter->font();
    font.setPointSize(m_fontSize);
    font.setBold(true);
    painter->setFont(font);

    // 4. Рисуем все подписи из матрицы
    for (const Label& label : labels) {
        QPointF screenPos = projectToScreen(label.worldPos);

        if (screenPos.x() < 0 || screenPos.x() > screenWidth ||
            screenPos.y() < 0 || screenPos.y() > screenHeight) continue;

        painter->setPen(label.color);
        QRectF rect(screenPos.x() - 30, screenPos.y() - 10, 60, 20);
        painter->drawText(rect, Qt::AlignCenter, label.text);
    }
}
