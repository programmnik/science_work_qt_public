#include "plane2d.h"
#include <GL/gl.h>
#include <GL/glu.h>

Plane2d::Plane2d()
{
    setFontSize(8);

    // Инициализация: стартовая ширина клетки = 1.0, breakpoint по умолчанию
    m_zoomController.init(0.4);
}

void Plane2d::visibleRange(CameraCore2D* cam2d, int screenWidth, int screenHeight,
                           float& minX, float& maxX, float& minY, float& maxY) const
{
    // const float cellWidth = static_cast<float>(m_zoomController.cellWidth());
    const QVector3D pos = cam2d->getCameraPos();
    const float aspect = screenWidth / static_cast<float>(screenHeight);

    // Полувысота видимой области = N клеток
    const float halfHeight = kCellsPerHalfHeight;
    const float halfWidth  = halfHeight * aspect;

    minX = pos.x() - halfWidth;
    maxX = pos.x() + halfWidth;
    minY = pos.y() - halfHeight;
    maxY = pos.y() + halfHeight;
}

QVector<Plane::Label> Plane2d::generateLabels(Camera* camera, int screenWidth, int screenHeight)
{
    QVector<Label> labels;

    if (m_labelMode == ModeNone || !camera) return labels;
    auto* cam2d = dynamic_cast<CameraCore2D*>(camera);
    if (!cam2d) return labels;

    float minX, maxX, minY, maxY;
    visibleRange(cam2d, screenWidth, screenHeight, minX, maxX, minY, maxY);

    // Шаг подписей = ширина клетки
    const float step = static_cast<float>(m_zoomController.cellWidth());
    const int cellToZero = static_cast<int>(m_zoomController.cellsToZero());
    const int seriesRatio = m_zoomController.seriesStep();
    const int64_t exponent = m_zoomController.currentExponent();

    auto addLabel = [&](const ComplexNumber& num, const QVector3D& worldPos, const QColor& color) {
        Label label;
        label.number   = num;
        label.worldPos = worldPos;
        label.text     = num.toString();
        label.color    = color;
        label.fontSize = m_fontSize;
        labels.append(label);
    };

    // ---- Режим: только оси ----
    if (m_labelMode == ModeAxesOnly) {

        float ratio = step * cellToZero;

        // Ось Re (y = 0): рисуем лейблы кратные step
        if (minY <= 0 && maxY >= 0) {
            int start = std::floor(minX / ratio);
            int end = maxX / ratio;
            for (int x = start; x <= end; x++) {
                if (x == 0) continue;
                addLabel(ComplexNumber(BigFloat(x * seriesRatio, exponent), BigFloat()), QVector3D(x * ratio, 0, 0),
                         QColor(255, 100, 100));
            }
        }

        // Ось Im (x = 0): рисуем лейблы кратные step
        if (minX <= 0 && maxX >= 0) {
            int start = std::floor(minY / ratio);
            int end = maxY / ratio;
            for (int y = start; y <= end; y ++) {
                if (y == 0) continue;
                addLabel(ComplexNumber(BigFloat(), BigFloat(y * seriesRatio, exponent)), QVector3D(0, y * ratio, 0),
                         QColor(100, 255, 100));
            }
        }

        // Ноль
        if (minX <= 0 && maxX >= 0 && minY <= 0 && maxY >= 0) {
            addLabel(ComplexNumber(), QVector3D(0, 0, 0),
                     QColor(255, 255, 255));
        }
    }

    // ---- Режим: все числа ----
    if (m_labelMode == ModeAll) {
        if (minX <= 0 && maxX >= 0 && minY <= 0 && maxY >= 0) {
            addLabel(ComplexNumber(), QVector3D(0, 0, 0), QColor(255, 255, 255));
        }

        float ratio = step * cellToZero;

        int startX = std::floor(minX / ratio);
        int endX   = maxX / ratio;
        int startY = std::floor(minY / ratio);
        int endY   = maxY / ratio;

        for (int x = startX; x <= endX; x ++) {
            for (int y = startY; y <= endY; y ++) {
                ComplexNumber num{BigFloat(x * seriesRatio, exponent), BigFloat(y * seriesRatio, exponent)};
                if (num.isZero()) continue;

                QVector3D worldPos(x * ratio, y * ratio, 0);
                addLabel(num, worldPos,
                         !num.isOnAxis() ? QColor(200, 200, 255, 180)
                         : num.getRe().isZero() ? QColor(100, 255, 100)
                         : QColor(255, 100, 100));
            }
        }
    }

    return labels;
}

void Plane2d::drawGrid(CameraCore2D* camera2d, int viewportWidth, int viewportHeight)
{
    if (!camera2d) return;

    const int cellToZero = static_cast<int>(m_zoomController.cellsToZero());

    float minX, maxX, minY, maxY;
    visibleRange(camera2d, viewportWidth, viewportHeight, minX, maxX, minY, maxY);

    const float step = static_cast<float>(m_zoomController.cellWidth());

    // Выравниваем стартовые позиции по сетке
    const float startX = std::floor(minX / step) * step;
    const float endX   = std::ceil (maxX / step) * step;
    const float startY = std::floor(minY / step) * step;
    const float endY   = std::ceil (maxY / step) * step;

    // Индекс линии = координата / step. startX/step — целое (после floor).
    const long i0x = static_cast<long>(std::llround(startX / step));
    const long i1x = static_cast<long>(std::llround(endX   / step));
    const long i0y = static_cast<long>(std::llround(startY / step));
    const long i1y = static_cast<long>(std::llround(endY   / step));

    auto drawGridLines = [&](bool vertical) {
        const long i0 = vertical ? i0x : i0y;
        const long i1 = vertical ? i1x : i1y;
        for (long i = i0; i <= i1; ++i) {
            // пропускаем оси (i == 0)
            if (i == 0) continue;

            const float coord = static_cast<float>(i) * step;
            const bool major  = (i % cellToZero == 0);

            if (major) {
                glLineWidth(2.0f);
                glColor3f(0.45f, 0.45f, 0.7f);   // ярче/толще
            } else {
                glLineWidth(1.0f);
                glColor3f(0.3f, 0.3f, 0.4f);   // обычная
            }

            if (vertical) {
                glVertex3f(coord, minY, 0);
                glVertex3f(coord, maxY, 0);
            } else {
                glVertex3f(minX, coord, 0);
                glVertex3f(maxX, coord, 0);
            }
        }
    };

    // Оси
    glColor3f(0.3f, 0.3f, 0.4f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    drawGridLines(true);
    drawGridLines(false);
    glEnd();

    glLineWidth(1.0f);

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

    QVector<Label> labels = generateLabels(cam2d, screenWidth, screenHeight);

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(m_fontSize);
    painter->setFont(font);

    // Пересчитываем видимый диапазон для проекции
    float minX, maxX, minY, maxY;
    visibleRange(cam2d, screenWidth, screenHeight, minX, maxX, minY, maxY);

    const float rangeX = maxX - minX;
    const float rangeY = maxY - minY;

    for (const Label& label : labels) {
        // Проекция мировой точки в экранные координаты
        const float nx = (label.worldPos.x() - minX) / rangeX;   // 0..1
        const float ny = (label.worldPos.y() - minY) / rangeY;   // 0..1

        const float screenX = nx * screenWidth;
        const float screenY = (1.0f - ny) * screenHeight;

        QPointF screenPos(screenX, screenY);

        if (screenPos.x() < 0 || screenPos.x() > screenWidth ||
            screenPos.y() < 0 || screenPos.y() > screenHeight) continue;

        QFontMetrics metrics(font);
        QRect textRect = metrics.boundingRect(label.text);

        int padding = 4;
        int width   = textRect.width()  + padding * 2;
        int height  = textRect.height() + padding * 2;

        QRectF rect(screenPos.x() - width, screenPos.y(), width, height);

        painter->setPen(label.color);
        painter->drawText(rect, Qt::AlignCenter, label.text);
    }
}
