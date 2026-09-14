#ifndef PLANE_H
#define PLANE_H

#include "../Camera/camera.h"
#include "Core/complexnumber.h"
#include <QString>
#include <QVector3D>
#include <QColor>
#include <QPainter>
#include <QVector>

class Plane
{
public:
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
    virtual ~Plane() = default;

    virtual void draw(Camera* camera, QPainter* painter, int screenWidth, int screenHeight) = 0;

    void setStepNumber(float step) { stepNumber = step; }
    void setLabelMode(LabelMode mode) { m_labelMode = mode; }
    void setFontSize(int fontSize){ m_fontSize = fontSize; }
    LabelMode getLabelMode() const { return m_labelMode; }

protected:
    // Генерация матрицы подписей
    virtual QVector<Label> generateLabels(Camera* camera, int screenWidth, int screenHeight) = 0;

    float stepNumber;
    float stepX;
    float stepY;
    LabelMode m_labelMode = ModeAll;
    int m_fontSize = 16;
};

#endif // PLANE_H
