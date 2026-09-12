#include "objectmanager.h"
#include <QDebug>

ObjectManager& ObjectManager::instance()
{
    static ObjectManager manager;
    return manager;
}

void ObjectManager::addObject(std::shared_ptr<SceneObject> object)
{
    if (!object) return;
    m_objects.append(object);
    emit objectAdded(object);
}

void ObjectManager::removeObject(SceneObject* object)
{
    for (int i = 0; i < m_objects.size(); ++i) {
        if (m_objects[i].get() == object) {
            auto removed = m_objects[i];
            m_objects.removeAt(i);
            emit objectRemoved(removed);
            return;
        }
    }
}

void ObjectManager::removeObjectAt(int index)
{
    if (index >= 0 && index < m_objects.size()) {
        auto removed = m_objects[index];
        m_objects.removeAt(index);
        emit objectRemoved(removed);
    }
}

void ObjectManager::clearAll()
{
    m_objects.clear();
    emit allObjectsCleared();
}

QVector<std::shared_ptr<SceneObject>> ObjectManager::getObjectsByType(SceneObject::ObjectType type) const
{
    QVector<std::shared_ptr<SceneObject>> result;
    for (const auto& obj : m_objects) {
        if (obj->getType() == type) {
            result.append(obj);
        }
    }
    return result;
}

QMap<SceneObject::ObjectType, QVector<std::shared_ptr<SceneObject>>> ObjectManager::getGroupedObjects() const
{
    QMap<SceneObject::ObjectType, QVector<std::shared_ptr<SceneObject>>> groups;
    for (const auto& obj : m_objects) {
        groups[obj->getType()].append(obj);
    }
    return groups;
}

std::shared_ptr<SceneObject> ObjectManager::getObjectAt(int index) const
{
    if (index >= 0 && index < m_objects.size()) {
        return m_objects[index];
    }
    return nullptr;
}
