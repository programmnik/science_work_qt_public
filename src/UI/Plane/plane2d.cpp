#include "plane2d.h"
#include <GL/gl.h>
#include <GL/glu.h>

Plane2d::Plane2d()
{
    setFontSize(8);
}

QVector<Plane::Label> Plane2d::generateLabels(Camera* camera, int screenWidth, int screenHeight){
    QVector<Label> labels;

    if (m_labelMode == ModeNone || !camera) return labels;
    auto* cam2d = dynamic_cast<CameraCore2D*>(camera);
    if (!cam2d) return labels;

    // Получаем параметры камеры
    float viewSize = cam2d->getViewSize() / cam2d->getZoom();
    QVector3D pos = cam2d->getCameraPos();

    float aspect = screenWidth / (float)screenHeight;

    float halfWidth = viewSize * aspect;
    float halfHeight = viewSize;

    float minX = pos.x() - halfWidth;
    float maxX = pos.x() + halfWidth;
    float minY = pos.y() - halfHeight;
    float maxY = pos.y() + halfHeight;

    // Определяем шаг для подписей
    float visibleWidth = maxX - minX;
    float visibleHeight = maxY - minY;
    float maxRange = std::max(visibleWidth, visibleHeight);

    float step = 1.0f;
    if (maxRange > 20.0f) step = 2.0f;
    if (maxRange > 50.0f) step = 5.0f;
    if (maxRange > 100.0f) step = 10.0f;

    auto addLabel = [&](const ComplexNumber& num, const QVector3D& worldPos, const QColor& color) {
        Label label;
        label.number = num;
        label.worldPos = worldPos;
        label.text = num.toString();
        label.color = color;
        label.fontSize = m_fontSize;
        labels.append(label);
    };

    // РЕЖИМ: только оси
    if (m_labelMode == ModeAxesOnly) {
        // ---- Реальная ось: (x, 0) ----
        if (minY <= 0 && maxY >= 0) {
            for (float x = std::floor(minX); x <= std::ceil(maxX); x += step) {
                if (std::abs(x) < 0.01f) continue;
                QVector3D worldPos(x, 0, 0);
                addLabel(ComplexNumber(x, 0.0f), worldPos, QColor(255, 100, 100));
            }
        }

        // ---- Мнимая ось: (0, y) ----
        if (minX <= 0 && maxX >= 0) {
            for (float y = std::floor(minY); y <= std::ceil(maxY); y += step) {
                if (std::abs(y) < 0.01f) continue;
                QVector3D worldPos(0, y, 0);
                addLabel(ComplexNumber(0.0f, y), worldPos, QColor(100, 255, 100));
            }
        }

        // ---- Ноль: (0, 0) ----
        if (minX <= 0 && maxX >= 0 && minY <= 0 && maxY >= 0) {
            addLabel(ComplexNumber(0.0f, 0.0f), QVector3D(0, 0, 0), QColor(255, 255, 255));
        }
    }

    // РЕЖИМ: все числа
    if (m_labelMode == ModeAll) {

        if (minX <= 0 && maxX >= 0 && minY <= 0 && maxY >= 0) {
            addLabel(ComplexNumber(0.0f, 0.0f), QVector3D(0, 0, 0), QColor(255, 255, 255));
        }

        for (float x = std::floor(minX); x <= std::ceil(maxX); x += step) {
            for (float y = std::floor(minY); y <= std::ceil(maxY); y += step) {
                ComplexNumber num(x, y);
                if (num.isZero()) continue;

                QVector3D worldPos(x, y, 0);

                addLabel(num, worldPos, !num.isOnAxis() ? QColor(200, 200, 255, 180)
                                   : std::abs(num.getRe()) < 0.001f ? QColor(100, 255, 100)
                                   : QColor(255, 100, 100));
            }
        }
    }

    return labels;
}

void Plane2d::drawGrid(CameraCore2D* camera2d, int viewportWidth, int viewportHeight)
{
    if (!camera2d) return;

    // Получаем параметры камеры
    float viewSize = camera2d->getViewSize() / camera2d->getZoom();
    QVector3D pos = camera2d->getCameraPos();

    float aspect = viewportWidth / (float)viewportHeight;

    float halfWidth = viewSize * aspect;
    float halfHeight = viewSize;

    float minX = pos.x() - halfWidth;
    float maxX = pos.x() + halfWidth;
    float minY = pos.y() - halfHeight;
    float maxY = pos.y() + halfHeight;

    float visibleWidth = maxX - minX;
    float visibleHeight = maxY - minY;
    float maxRange = std::max(visibleWidth, visibleHeight);

    float step = 0.5f;
    if (maxRange > 20.0f) step = 1.0f;
    if (maxRange > 50.0f) step = 2.0f;
    if (maxRange > 100.0f) step = 5.0f;
    if (maxRange > 200.0f) step = 10.0f;

    float startX = std::floor(minX / step) * step;
    float endX = std::ceil(maxX / step) * step;
    float startY = std::floor(minY / step) * step;
    float endY = std::ceil(maxY / step) * step;

    // Сетка
    glColor3f(0.3f, 0.3f, 0.4f);
    glBegin(GL_LINES);

    for (float x = startX; x <= endX; x += step) {
        if (std::abs(x) < 0.01f) continue;
        glVertex3f(x, minY, 0);
        glVertex3f(x, maxY, 0);
    }

    for (float y = startY; y <= endY; y += step) {
        if (std::abs(y) < 0.01f) continue;
        glVertex3f(minX, y, 0);
        glVertex3f(maxX, y, 0);
    }
    glEnd();

    // Оси
    glLineWidth(2.0f);
    glBegin(GL_LINES);

    if (minY <= 0 && maxY >= 0) {
        glColor3f(1.0f, 0.2f, 0.2f);
        glVertex3f(minX, 0, 0);
        glVertex3f(maxX, 0, 0);
    }

    if (minX <= 0 && maxX >= 0) {
        glColor3f(0.2f, 1.0f, 0.2f);
        glVertex3f(0, minY, 0);
        glVertex3f(0, maxY, 0);
    }

    glEnd();
    glLineWidth(1.0f);
}

void Plane2d::draw(Camera* camera, QPainter* painter, int screenWidth, int screenHeight)
{
    Q_UNUSED(painter);

    if (!camera) return;

    auto* cam2d = dynamic_cast<CameraCore2D*>(camera);
    if (!cam2d) return;

    drawGrid(cam2d, screenWidth, screenHeight);

    if (!painter) return;

    // Генерируем подписи
    QVector<Label> labels = generateLabels(cam2d, screenWidth, screenHeight);

    // Настройка шрифта
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(m_fontSize);
    painter->setFont(font);

    // Рисуем все подписи
    for (const Label& label : labels) {
        // В 2D режиме проецируем точку на экран через камеру
        // Для 2D используем простую проекцию (без gluProject)
        float viewSize = cam2d->getViewSize() / cam2d->getZoom();
        QVector3D cameraPos = cam2d->getCameraPos();
        float aspect = screenWidth / (float)screenHeight;

        float halfWidth = viewSize * aspect;
        float halfHeight = viewSize;

        // Преобразуем мировые координаты в экранные
        float screenX = (label.worldPos.x() - (cameraPos.x() - halfWidth)) / (2 * halfWidth) * screenWidth;
        float screenY = (1.0f - (label.worldPos.y() - (cameraPos.y() - halfHeight)) / (2 * halfHeight)) * screenHeight;

        QPointF screenPos(screenX, screenY);

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
        QRectF rect(screenPos.x() - width,
                    screenPos.y(),
                    width, height);

        painter->setPen(label.color);
        painter->drawText(rect, Qt::AlignCenter, label.text);
    }
}
