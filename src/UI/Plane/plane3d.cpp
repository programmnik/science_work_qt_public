#include "plane3d.h"
#include "Core/constants.h"
#include <cmath>
#include <QFont>
#include <GL/gl.h>
#include <GL/glu.h>

Plane3d::Plane3d()
    : viewDistance(100.0f)
    , viewNumberDistance(20.0f)
{}

void Plane3d::calculateVisibleRange(const Point &cameraPoint,
                                    float &minX, float &maxX,
                                    float &minY, float &maxY,
                                    float view)
{
    float camX = cameraPoint.getX();
    float camY = cameraPoint.getY();

    minX = camX - view;
    maxX = camX + view;
    minY = camY - view;
    maxY = camY + view;
}

QPointF Plane3d::projectToScreen(const QVector3D& worldPos)
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

bool Plane3d::isPointInFrontOfCamera(const QVector3D& point, const QVector3D& cameraPos, const QVector3D& cameraFront)
{
    return QVector3D::dotProduct(point - cameraPos, cameraFront) > 0;
}

void Plane3d::drawGrid(const Point& cameraPoint)
{
    float minX, maxX, minY, maxY;
    calculateVisibleRange(cameraPoint, minX, maxX, minY, maxY, viewDistance);

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

void Plane3d::drawAxes(const Point& cameraPoint)
{
    float minX, maxX, minY, maxY;
    calculateVisibleRange(cameraPoint, minX, maxX, minY, maxY, viewDistance);

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

QVector<Plane::Label> Plane3d::generateLabels(Camera* camera, int screenWidth, int screenHeight)
{
    QVector3D cameraPos = camera->getCameraPos();
    QVector3D cameraFront = camera->getCameraFront();
    QVector<Label> labels;

    if (m_labelMode == ModeNone) return labels;

    float minX, maxX, minY, maxY;
    calculateVisibleRange(*camera->getPoint(), minX, maxX, minY, maxY, viewNumberDistance);

    auto isVisible = [&](const QVector3D& pos) -> bool {
        if (!isPointInFrontOfCamera(pos, cameraPos, cameraFront)) return false;
        QPointF screenPos = projectToScreen(pos);
        return (screenPos.x() >= 0 && screenPos.x() <= screenWidth &&
                screenPos.y() >= 0 && screenPos.y() <= screenHeight);
    };

    auto calculateFontSize = [&](const QVector3D& pos) -> float {
        // Расстояние от камеры до точки в плоскости XY
        float dx = pos.x() - cameraPos.x();
        float dy = pos.y() - cameraPos.y();
        float distance = std::sqrt(dx*dx + dy*dy);

        // Базовая дистанция, на которой шрифт имеет базовый размер
        float baseDistance = 5.0f;

        // Минимальный и максимальный размер шрифта
        float minSize = 1.0f;
        float maxSize = m_fontSize;  // Базовый размер из класса

        if (distance < 0.1f) return maxSize;  // Слишком близко - максимальный размер

        // Логарифмическое масштабирование
        float scale = baseDistance / (baseDistance + distance * 0.3f);

        float size = maxSize * scale;
        return std::clamp(size, minSize, maxSize);
    };

    auto addLabel = [&](const ComplexNumber& num, const QVector3D& pos, const QColor& color) {
        Label label;
        label.number = num;
        label.worldPos = pos;
        label.text = num.toString();
        label.color = color;
        label.fontSize = calculateFontSize(pos);
        labels.append(label);
    };

    auto drawZero = [&](){
        if(minY <= 0 && maxY >= 0 && minX <= 0 && maxX >= 0){
            QVector3D originPos(0, 0, 0);
            if (isVisible(originPos)) {
                addLabel(ComplexNumber(), originPos, QColor(255, 255, 255));
            }
        }
    };

    // РЕЖИМ: только оси
    if (m_labelMode == ModeAxesOnly) {

        drawZero();

        // ---- Реальная ось: (x, 0) ----
        if(minY <= 0 && maxY >= 0){
            for (float x = std::floor(minX); x <= std::ceil(maxX); x += stepNumber) {
                if (std::abs(x) < 0.01f) continue;
                QVector3D pos(x, 0, 0);
                if (isVisible(pos)) addLabel(ComplexNumber(BigFloat(x), BigFloat()), pos, QColor(255, 100, 100));
            }
        }

        // ---- Мнимая ось: (0, y) ----
        if(minX <= 0 && maxX >= 0){
            for (float y = std::floor(minY); y <= std::ceil(maxY); y += stepNumber) {
                if (std::abs(y) < 0.01f) continue;
                QVector3D pos(0, y, 0);
                if (isVisible(pos)) addLabel(ComplexNumber(BigFloat(), BigFloat(y)), pos, QColor(100, 255, 100));
            }
        }
    }

    // РЕЖИМ: все числа
    if (m_labelMode == ModeAll) {

        drawZero();

        for (float x = std::floor(minX); x <= std::ceil(maxX); x += stepNumber) {
            for (float y = std::floor(minY); y <= std::ceil(maxY); y += stepNumber) {
                ComplexNumber num{BigFloat(x), BigFloat(y)};

                if (num.isZero()) continue;

                QVector3D pos(x, y, 0);

                if (!isPointInFrontOfCamera(pos, cameraPos, cameraFront)) continue;

                QPointF screenPos = projectToScreen(pos);
                if (screenPos.x() < 0 || screenPos.x() > screenWidth ||
                    screenPos.y() < 0 || screenPos.y() > screenHeight) continue;

                addLabel(num, pos, !num.isOnAxis() ? QColor(200, 200, 255, 180)
                                   : num.getRe().isZero() ? QColor(100, 255, 100)
                                   : QColor(255, 100, 100));
            }
        }
    }

    return labels;
}

void Plane3d::draw(Camera* camera, QPainter* painter, int screenWidth, int screenHeight){

    Point* cameraPoint = camera->getPoint();
    QVector3D cameraPos = camera->getCameraPos();

    // 1. Рисуем сетку и оси через OpenGL
    drawGrid(*cameraPoint);
    drawAxes(*cameraPoint);

    if (!painter) return;

    // 2. Генерируем матрицу подписей
    QVector<Label> labels = generateLabels(camera, screenWidth, screenHeight);

    // 3. Настройка шрифта
    QFont font = painter->font();
    font.setBold(true);

    // 4. Рисуем все подписи из матрицы
    for (const Label& label : labels) {
        font.setPointSize(label.fontSize);
        painter->setFont(font);
        QPointF screenPos = projectToScreen(label.worldPos);

        if (screenPos.x() < 0 || screenPos.x() > screenWidth ||
            screenPos.y() < 0 || screenPos.y() > screenHeight) continue;

        // Вычисляем реальный размер текста
        QFontMetrics metrics(font);
        QRect textRect = metrics.boundingRect(label.text);

        // Добавляем небольшие отступы (padding)
        int padding = 4;
        int width = textRect.width() + padding * 2;
        int height = textRect.height() + padding * 2;

        // Позиционируем относительно точки
        QRectF rect(screenPos.x() - width/2,
                    screenPos.y() - height/2,
                    width,
                    height);

        painter->setPen(label.color);
        painter->drawText(rect, Qt::AlignCenter, label.text);
    }
}
