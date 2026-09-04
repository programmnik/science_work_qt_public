#ifndef SPHEREPOINT_H
#define SPHEREPOINT_H

#include <QVector3D>
#include <QVector2D>
#include <QColor>
#include <QString>

class SpherePoint
{
public:
    SpherePoint();
    SpherePoint(const QVector3D& spherePos);
    SpherePoint(float x, float y, float z);

    // Координаты на сфере
    QVector3D getSpherePosition() const { return m_spherePos; }
    void setSpherePosition(const QVector3D& pos) { m_spherePos = pos; }

    // Проекция на плоскость (стереографическая)
    QVector3D getPlaneProjection() const;
    void setPlaneProjection(const QVector3D& planePos);

    // Ортогональная проекция (просто x,y)
    QVector2D getOrthogonalProjection() const { return QVector2D(m_spherePos.x(), m_spherePos.y()); }

    // Проверка, находится ли точка на сфере
    bool isValid() const { return m_spherePos.length() > 0.001f; }

    // Цвет (QColor)
    void setColor(const QColor& color) { m_color = color; }
    QColor getColor() const { return m_color; }

    // Подпись
    void setLabel(const QString& label) { m_label = label; }
    QString getLabel() const { return m_label; }

private:
    QVector3D m_spherePos;      // Позиция на сфере (3D)
    QColor m_color;             // Цвет точки (QColor)
    QString m_label;            // Подпись
    bool m_hasPlaneProjection;  // Есть ли проекция на плоскость
};

#endif // SPHEREPOINT_H
