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
    SpherePoint(const QVector3D& spherePos);  // worldPos в мировых координатах
    SpherePoint(float x, float y, float z);

    // Координаты на сфере в МИРОВЫХ координатах
    QVector3D getSpherePosition() const { return m_spherePos; }
    void setSpherePosition(const QVector3D& pos) { m_spherePos = pos; }

    // Проекция на плоскость (через уравнение прямой)
    QVector3D getPlaneProjection(const QVector3D& northPole) const;

    // Ортогональная проекция (просто x,y)
    QVector2D getOrthogonalProjection() const { return QVector2D(m_spherePos.x(), m_spherePos.y()); }

    // Проверка
    bool isValid() const { return m_spherePos.length() > 0.001f; }

    // Цвет
    void setColor(const QColor& color) { m_color = color; }
    QColor getColor() const { return m_color; }

    // Подпись
    void setLabel(const QString& label) { m_label = label; }
    QString getLabel() const { return m_label; }

private:
    QVector3D m_spherePos;      // Позиция на сфере в МИРОВЫХ координатах
    QColor m_color;
    QString m_label;
    bool m_hasPlaneProjection;
};

#endif // SPHEREPOINT_H
