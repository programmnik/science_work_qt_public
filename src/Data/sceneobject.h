#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <QString>
#include <QColor>

class SceneObject
{
public:
    enum ObjectType {
        TypeSphere,
        TypePlane,
        TypeProjection,
        TypePoint,
        TypeCustom
    };

    SceneObject(ObjectType type, const QString& name = "");
    virtual ~SceneObject() = default;

    // Геттеры
    ObjectType getType() const { return m_type; }
    QString getTypeName() const;
    QString getName() const { return m_name; }
    QColor getColor() const { return m_color; }
    bool isVisible() const { return m_visible; }
    bool isSelected() const { return m_selected; }

    // Сеттеры
    void setName(const QString& name) { m_name = name; }
    void setColor(const QColor& color) { m_color = color; }
    void setVisible(bool visible) { m_visible = visible; }
    void setSelected(bool selected) { m_selected = selected; }

    virtual QString getDescription() const;

protected:
    ObjectType m_type;
    QString m_name;
    QColor m_color;
    bool m_visible = true;
    bool m_selected = false;
};

#endif // SCENEOBJECT_H
