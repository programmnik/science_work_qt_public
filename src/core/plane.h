#ifndef PLANE_H
#define PLANE_H

#include "point.h"
#include "cameracore.h"
#include <QString>
#include <QVector3D>
#include <QColor>
#include <QPainter>
#include <QVector>

class Plane
{
public:
    // Структура для комплексного числа
    struct ComplexNumber {
        float re;
        float im;

        QString toString() const {
            int reInt = qRound(re);
            int imInt = qRound(im);

            if (reInt == 0 && imInt == 0) return "0";
            if (reInt == 0) {
                if (imInt == 1) return "i";
                if (imInt == -1) return "-i";
                return QString::number(imInt) + "i";
            }
            if (imInt == 0) {
                return QString::number(reInt);
            }

            QString imStr = (imInt == 1) ? "i" : (imInt == -1) ? "-i" : QString::number(imInt) + "i";

            if (imInt > 0) {
                return QString::number(reInt) + "+" + imStr;
            } else {
                return QString::number(reInt) + imStr;
            }
        }

        bool isOnAxis() const {
            return std::abs(re) < 0.001f || std::abs(im) < 0.001f;
        }

        bool isZero() const {
            return std::abs(re) < 0.001f && std::abs(im) < 0.001f;
        }
    };

    // Структура для подписи
    struct Label {
        ComplexNumber number;
        QVector3D worldPos;
        QString text;
        QColor color;
        int fontSize;
    };

    enum LabelMode {
        ModeNone = 0,
        ModeAxesOnly = 1,
        ModeAll = 2
    };

    Plane();

    void draw(const Point& cameraPoint,
              const QVector3D& cameraFront,
              QPainter* painter,
              int screenWidth, int screenHeight);

    void setViewDistance(float distance) { viewDistance = distance; }
    void setViewNumberDistance(float numDistance) { viewNumberDistance = numDistance; }
    void setStepNumber(float step) { stepNumber = step; }
    void setLabelMode(LabelMode mode) { m_labelMode = mode; }
    LabelMode getLabelMode() const { return m_labelMode; }

private:
    void calculateVisibleRange(const Point& cameraPoint,
                               float& minX, float& maxX,
                               float& minY, float& maxY);

    void drawGrid(const Point& cameraPoint);
    void drawAxes(const Point& cameraPoint);

    // Генерация матрицы подписей
    QVector<Label> generateLabels(const QVector3D& cameraPos,
                                  const QVector3D& cameraFront,
                                  int screenWidth, int screenHeight);

    // Проекция 3D -> 2D через OpenGL
    QPointF projectToScreen(const QVector3D& worldPos);

    bool isPointInFrontOfCamera(const QVector3D& point,
                                const QVector3D& cameraPos,
                                const QVector3D& cameraFront);

    float viewDistance;
    float viewNumberDistance;
    float stepNumber;
    float stepX;
    float stepY;
    LabelMode m_labelMode = ModeAll;

    float m_labelOffset = 0.15f;
    int m_fontSize = 11;
};

#endif // PLANE_H
