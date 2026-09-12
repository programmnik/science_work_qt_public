#include "sceneobject.h"

SceneObject::SceneObject(ObjectType type, const QString& name)
    : m_type(type)
    , m_name(name.isEmpty() ? getTypeName() : name)
{
    switch (type) {
    case TypeSphere:    m_color = QColor(30, 144, 255); break;
    case TypePlane:     m_color = QColor(100, 200, 200); break;
    case TypeProjection:m_color = QColor(255, 165, 0); break;
    case TypePoint:     m_color = QColor(255, 255, 0); break;
    default:            m_color = QColor(200, 200, 200); break;
    }
}

QString SceneObject::getTypeName() const
{
    switch (m_type) {
    case TypeSphere:    return "Сфера";
    case TypePlane:     return "Плоскость";
    case TypeProjection:return "Проекция";
    case TypePoint:     return "Точка";
    default:            return "Объект";
    }
}

QString SceneObject::getDescription() const
{
    return QString("%1: %2").arg(getTypeName()).arg(m_name);
}
